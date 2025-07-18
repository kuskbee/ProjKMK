// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginClientSubsystem.h"

//WinSock (hton, ntoh)
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Windows/HideWindowsPlatformTypes.h"

#include "Networking.h"
#include "SocketSubsystem.h"
//#include "Sockets.h"
//#include "Common/TcpSocketBuilder.h"
#include "HAL/PlatformMisc.h"

#include "UObject/EnumProperty.h"

#include "Utils.h"

void ULoginClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Socket = nullptr;
}

void ULoginClientSubsystem::Deinitialize()
{
	DisconnectFromServer();
	Super::Deinitialize();
}

void ULoginClientSubsystem::ConnectToServer(const FString& HostOrIp, int32 Port)
{
	if (Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already connected."));
		return;
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Socket Subsystem"));
		return;
	}


	/*──────────────────────────────────────────────
	*  Host 문자열 → FInternetAddr 변환
	*    (IPv4 parsing → 실패 시 DNS lookup)
	*──────────────────────────────────────────────*/
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	FIPv4Address IPv4Addr;
	if (FIPv4Address::Parse(HostOrIp, IPv4Addr))
	{
		Addr->SetIp(IPv4Addr.Value);
	}
	else
	{
		ESocketErrors Result = SocketSubsystem->GetHostByName(
			TCHAR_TO_ANSI(*HostOrIp),
			*Addr
		);
		if (Result == ESocketErrors::SE_NO_ERROR)
		{
			FString ResolvedIP = Addr->ToString(false);
			UE_LOG(LogTemp, Warning, TEXT("DNS resolved Addr [%s] to IP: [%s]"), *HostOrIp, *ResolvedIP);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] DNS resolution Failed for %s (Error: %d)"),
				*HostOrIp, (int32)Result);
			return;
		}
	}

	// 포트 설정
	Addr->SetPort(Port);

	/*──────────────────────────────────────────────
	 *  소켓 생성 ‧ 연결 시도
	 *──────────────────────────────────────────────*/
	Socket = FTcpSocketBuilder(TEXT("LoginClientSocket")).AsBlocking().Build();

	if (!Socket || !Socket->Connect(*Addr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect to server: %s:%d"), *HostOrIp, Port);

		if (Socket)
		{
			SocketSubsystem->DestroySocket(Socket);
			Socket = nullptr;
		}
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Connected to Server: %s:%d"), *HostOrIp, Port);

	/*──────────────────────────────────────────────
	 *  네트워크 수신 스레드 생성
	 *──────────────────────────────────────────────*/
	bRunRecv = true;
	LoginRecvThread = MakeUnique<std::thread>(&ULoginClientSubsystem::RecvLoop, this);

	/*──────────────────────────────────────────────
	 *  네트워크 폴링 타이머 시작
	 *    (주기적으로 수신 패킷 큐 체크)
	 *──────────────────────────────────────────────*/
	GetWorld()->GetTimerManager().SetTimer(NetworkTimerHandle, this, &ULoginClientSubsystem::NetworkPolling, 0.1f, true);
}

void ULoginClientSubsystem::DisconnectFromServer()
{
	GetWorld()->GetTimerManager().ClearTimer(NetworkTimerHandle);

	bRunRecv = false;
	if (Socket)
	{
		Socket->Shutdown(ESocketShutdownMode::ReadWrite);
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Disconnected from server."));
	}

	if (LoginRecvThread && LoginRecvThread->joinable())
	{
		LoginRecvThread->join();
	}
	LoginRecvThread.Reset();
}

bool ULoginClientSubsystem::IsConnect()
{
	if (Socket && (Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected))
	{
		return true;
	}

	return false;
}

void ULoginClientSubsystem::NetworkPolling()
{
	if (!Socket) return;

	TArray<uint8> Buf;
	while (RecvPackets.Dequeue(Buf))
	{
		if (Buf.Num() == 0) //  워커 종료 신호
		{
			DisconnectFromServer();
			return;
		}

		// FlatBuffer 검증
		flatbuffers::Verifier verifier(
			reinterpret_cast<const uint8_t*>(Buf.GetData()), Buf.Num());

		if (!LoginProtocol::VerifyMessageEnvelopeBuffer(verifier))
		{
			UE_LOG(LogTemp, Error, TEXT("[VerifyMessage] Invalid FlatBuffer received"));
			continue;
		}

		// 메시지 디스패치
		ProcessPacket(Buf);
	}
}

void ULoginClientSubsystem::RecvLoop()
{
	FPlatformProcess::SetThreadName(TEXT("LoginRecv"));
	FTimespan PollTimeout = FTimespan::FromMilliseconds(100);
	constexpr size_t READ_CHUNK = 64 * 1024; // 64KB

	TArray<uint8_t> HeaderBuffer;
	int32 HeaderSize = (int32)sizeof(uint32_t);
	HeaderBuffer.SetNumUninitialized(HeaderSize, EAllowShrinking::No);

	while (bRunRecv && Socket)
	{

		// 데이터 기다리기 (100 ms 타임아웃 → 다른 스레드가 bRunRecv를 내렸는지 체크)
		if (!Socket->Wait(ESocketWaitConditions::WaitForRead, PollTimeout))
			continue;

		// 1. 메시지 길이 (4바이트) 수신
		uint32_t networkMessageSize = 0;

		int ReadBytes = 0;
		//bool bRecv = Socket->Recv(HeaderBuffer.GetData(), HeaderSize, ReadBytes);
		bool bRecv = RecvAllBlocking(HeaderBuffer, HeaderSize);
		if (bRecv == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Receive message size failed."));
			break;
		}

		int MessageSize = 0;
		FMemory::Memcpy(&networkMessageSize, HeaderBuffer.GetData(), sizeof(uint32_t));
		MessageSize = ntohl(networkMessageSize); // 호스트 바이트 순서로 변환

		if (MessageSize == 0 || MessageSize > READ_CHUNK)
		{
			UE_LOG(LogTemp, Error, TEXT("[RecvLoop] Bad pkt size"));
			break;
		}

		// 2. 실제 메시지 데이터 수신
		TArray<uint8> RecvBuf;
		RecvBuf.SetNumUninitialized(MessageSize, EAllowShrinking::No);
		bRecv = RecvAllBlocking(RecvBuf, MessageSize);
		if (bRecv == false)
		{
			UE_LOG(LogTemp, Error, TEXT("[RecvLoop] Receive message data failed."));
			break;
		}

		RecvPackets.Enqueue(MoveTemp(RecvBuf)); // 게임 스레드로 전달
	}

	RecvPackets.Enqueue(TArray<uint8>()); // 종료 알림 (빈 배열)
}

bool ULoginClientSubsystem::SendFlatBufferMessage(flatbuffers::FlatBufferBuilder& Builder)
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] Failed SendFlatBufferMessage - Invalid Socket!"));
		return false;
	}

	uint32 MessageSize = Builder.GetSize();
	uint32 NetworkMessageSize = htonl(MessageSize);
	int SentBytes = 0;
	if (false == Socket->Send((uint8*)&NetworkMessageSize, sizeof(NetworkMessageSize), SentBytes))
	{
		UE_LOG(LogTemp, Error, TEXT("Send message size failed."));
		return false;
	}

	SentBytes = 0;
	if (false == Socket->Send((uint8*)Builder.GetBufferPointer(), MessageSize, SentBytes))
	{
		UE_LOG(LogTemp, Error, TEXT("Send message data failed."));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SEND] %d bytes to Server"), MessageSize);

	return true;
}

bool ULoginClientSubsystem::ReceiveFlatBufferMessage(TArray<uint8_t>& RecvBuf, uint32_t& outMessageSize)
{
	// 1. 메시지 길이 (4바이트) 수신
	uint32_t networkMessageSize = 0;
	TArray<uint8_t> HeaderBuffer;
	int32 HeaderSize = (int32)sizeof(uint32_t);
	HeaderBuffer.AddZeroed(HeaderSize);

	int ReadBytes = 0;
	bool bRecv = RecvAllBlocking(HeaderBuffer, HeaderSize);
	if (bRecv == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Receive message size failed."));
		return false;
	}

	FMemory::Memcpy(&networkMessageSize, HeaderBuffer.GetData(), sizeof(uint32_t));
	outMessageSize = ntohl(networkMessageSize); // 호스트 바이트 순서로 변환

	// 2. 실제 메시지 데이터 수신

	RecvBuf.SetNumUninitialized(outMessageSize);
	bRecv = RecvAllBlocking(RecvBuf, outMessageSize);
	if (bRecv == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Receive message data failed."));
		return false;
	}
	return true;
}

bool ULoginClientSubsystem::RecvAllBlocking(TArray<uint8_t>& RecvBuf, int32 RecvBufLen)
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] Failed RecvAllBlocking - Invalid Socket!"));
		return false;
	}

	int32 Received = 0;
	while (Received < RecvBufLen)
	{
		int32 ReadBytes = 0;

		bool bRecv = Socket->Recv(RecvBuf.GetData() + Received, RecvBufLen - Received, ReadBytes);
		if (bRecv == false)
		{
			int32 Err = ISocketSubsystem::Get()->GetLastErrorCode();
			if (Err == SE_EWOULDBLOCK)  // 아직 일부만 도착
			{
				continue;
			}

			if (ReadBytes == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("[RecvAll] Server Socket disconnected."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("RecvAll failed: [%d]"), Err);
			}
			return false;
		}
		if (ReadBytes == 0) //graceful close
		{
			return false;
		}

		Received += ReadBytes;
	}
	return true;
}

void ULoginClientSubsystem::ProcessPacket(TArray<uint8_t>& RecvBuf)
{
	const LoginProtocol::MessageEnvelope* MsgEnvelope = LoginProtocol::GetMessageEnvelope(RecvBuf.GetData());
	if (!MsgEnvelope)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProcessPacket] Failed to GetMessageEnvelope"));
		return;
	}

	switch (MsgEnvelope->body_type())
	{
	case LoginProtocol::Payload::S2C_LoginResponse:
	{
		ProcessLoginResponse(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_SignUpResponse:
	{
		ProcessSignUpResponse(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_PlayerListResponse:
	{
		ProcessPlayerListResponse(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_PlayerInOutLobby:
	{
		ProcessPlayerInOutLobby(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_PlayerChangeState:
	{
		ProcessPlayerChangeState(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_CountdownStartGame:
	{
		ProcessCountdownStartGame(MsgEnvelope);
		break;
	}
	case LoginProtocol::Payload::S2C_StartGame:
	{
		ProcessStartGame(MsgEnvelope);
		break;
	}
	default:
		UE_LOG(LogTemp, Error, TEXT("Unknown payload type [%d]"), static_cast<uint8_t>(MsgEnvelope->body_type()));
		break;
	}
}

void ULoginClientSubsystem::ProcessLoginResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_LoginResponse* LoginRes = MsgEnvelope->body_as_S2C_LoginResponse();
	const ELoginServerErrorCode ErrCode = static_cast<ELoginServerErrorCode>(LoginRes->error_code());

	const char* Utf8Nickname = LoginRes->nickname()->c_str();
	const FString Nickname = FString(UTF8_TO_TCHAR(Utf8Nickname));

	const char* Utf8Token = LoginRes->session_token()->c_str();
	const FString SessionToken = FString(UTF8_TO_TCHAR(Utf8Token));


	OnLoginResponseDelegate.Broadcast(ErrCode, Nickname, SessionToken);
}

void ULoginClientSubsystem::ProcessSignUpResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_SignUpResponse* SignUpRes = MsgEnvelope->body_as_S2C_SignUpResponse();
	const ELoginServerErrorCode ErrCode = static_cast<ELoginServerErrorCode>(SignUpRes->error_code());

	OnSignUpResponseDelegate.Broadcast(ErrCode);
}

void ULoginClientSubsystem::ProcessPlayerListResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_PlayerListResponse* PlayerListRes = MsgEnvelope->body_as_S2C_PlayerListResponse();

	TArray<FPlayerInfo> PlayerInfos;

	UE_LOG(LogTemp, Warning, TEXT("[PlayerList] Received Player List, Num : %d"), PlayerListRes->players()->size());

	for (auto Player : *PlayerListRes->players())
	{
		const char* Utf8UserId = Player->user_id()->c_str();
		const FString UserId = FString(UTF8_TO_TCHAR(Utf8UserId));

		const char* Utf8Nickname = Player->nickname()->c_str();
		const FString Nickname = FString(UTF8_TO_TCHAR(Utf8Nickname));

		EEntryState PlyState = static_cast<EEntryState>(Player->state());

		PlayerInfos.Add(FPlayerInfo(UserId, Nickname, PlyState));

		UE_LOG(LogTemp, Warning, TEXT("[PlayerList] UserId : %s, NickName : %s, State : %s"),
			*UserId, *Nickname, *StaticEnum<EEntryState>()->GetNameStringByValue(static_cast<int64>(PlyState)));
	}

	UE_LOG(LogTemp, Warning, TEXT("[PlayerList] Received Player List, Num : %d"), PlayerInfos.Num());

	OnPlayerListResponseDelegate.Broadcast(PlayerInfos);
}

void ULoginClientSubsystem::ProcessPlayerInOutLobby(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_PlayerInOutLobby* PlayerInOutRes = MsgEnvelope->body_as_S2C_PlayerInOutLobby();

	const char* Utf8UserId = PlayerInOutRes->player()->user_id()->c_str();
	const FString UserId = FString(UTF8_TO_TCHAR(Utf8UserId));

	const char* Utf8Nickname = PlayerInOutRes->player()->nickname()->c_str();
	const FString Nickname = FString(UTF8_TO_TCHAR(Utf8Nickname));

	EEntryState PlyState = static_cast<EEntryState>(PlayerInOutRes->player()->state());

	bool bIsJoin = PlayerInOutRes->is_join();

	UE_LOG(LogTemp, Warning, TEXT("[ProcessPlayerInOutLobby] %s IsJoin [%d]"), *Nickname, bIsJoin);

	OnPlayerInOutLobbyDelegate.Broadcast(FPlayerInfo(UserId, Nickname, PlyState), bIsJoin);
}

void ULoginClientSubsystem::ProcessPlayerChangeState(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_PlayerChangeState* ChangeStateRes = MsgEnvelope->body_as_S2C_PlayerChangeState();

	const char* Utf8UserId = ChangeStateRes->user_id()->c_str();
	const FString UserId = FString(UTF8_TO_TCHAR(Utf8UserId));

	EEntryState PlyState = static_cast<EEntryState>(ChangeStateRes->state());

	OnPlayerChangeStateDelegate.Broadcast(UserId, PlyState);
}

void ULoginClientSubsystem::ProcessCountdownStartGame(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_CountdownStartGame* CountdownRes = MsgEnvelope->body_as_S2C_CountdownStartGame();

	bool bIsStart = CountdownRes->is_start();
	int RemainSeconds = CountdownRes->countdown();

	OnCountdownStartGameDelegate.Broadcast(bIsStart, RemainSeconds);
}

void ULoginClientSubsystem::ProcessStartGame(const LoginProtocol::MessageEnvelope* MsgEnvelope)
{
	const LoginProtocol::S2C_StartGame* StartGameRes = MsgEnvelope->body_as_S2C_StartGame();

	const char* Utf8DediIp = StartGameRes->dedi_ip_address()->c_str();
	const FString DediIp = FString(UTF8_TO_TCHAR(Utf8DediIp));
	const int DediPort = StartGameRes->dedi_port();

	FString ResultDediIp;

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Socket Subsystem"));
		return;
	}
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	FIPv4Address IPv4Addr;
	if (FIPv4Address::Parse(DediIp, IPv4Addr))
	{
		ResultDediIp = DediIp;
	}
	else
	{
		ESocketErrors Result = SocketSubsystem->GetHostByName(
			TCHAR_TO_ANSI(*DediIp),
			*Addr
		);
		if (Result == ESocketErrors::SE_NO_ERROR)
		{
			ResultDediIp = Addr->ToString(false);
			UE_LOG(LogTemp, Warning, TEXT("DNS resolved Addr [%s] to IP: [%s]"), *DediIp, *ResultDediIp);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] Failed Start Game!, DNS resolution Failed for %s (Error: %d)"),
				*DediIp, (int32)Result);
			return;
		}
	}

	OnStartGameDelegate.Broadcast(ResultDediIp, DediPort);
}

void ULoginClientSubsystem::SendLoginRequest(const FString& UserId, const FString& Password)
{
	std::string UserIdCharBuf = TCHAR_TO_UTF8(*UserId);
	std::string PasswordCharBuf = TCHAR_TO_UTF8(*Password);

	flatbuffers::FlatBufferBuilder Builder;
	auto UserIdOffset = Builder.CreateString(UserIdCharBuf);
	auto PasswordOffset = Builder.CreateString(PasswordCharBuf);
	auto BodyOffset = LoginProtocol::CreateC2S_LoginRequest(
		Builder,
		UserIdOffset,
		PasswordOffset
	);

	auto SendMsgData = LoginProtocol::CreateMessageEnvelope(
		Builder,
		GetTimeStamp(),
		LoginProtocol::Payload::C2S_LoginRequest,
		BodyOffset.Union()
	);
	Builder.Finish(SendMsgData);
	SendFlatBufferMessage(Builder);
}

void ULoginClientSubsystem::SendSignUpRequest(const FString& UserId, const FString& Password, const FString& Nickname)
{
	std::string UserIdCharBuf = TCHAR_TO_UTF8(*UserId);
	std::string PasswordCharBuf = TCHAR_TO_UTF8(*Password);
	std::string NicknameCharBuf = TCHAR_TO_UTF8(*Nickname);

	flatbuffers::FlatBufferBuilder Builder;
	auto UserIdOffset = Builder.CreateString(UserIdCharBuf);
	auto PasswordOffset = Builder.CreateString(PasswordCharBuf);
	auto NicknameOffset = Builder.CreateString(NicknameCharBuf);
	auto BodyOffset = LoginProtocol::CreateC2S_SignUpRequest(
		Builder,
		UserIdOffset,
		PasswordOffset,
		NicknameOffset
	);

	auto SendMsgData = LoginProtocol::CreateMessageEnvelope(
		Builder,
		GetTimeStamp(),
		LoginProtocol::Payload::C2S_SignUpRequest,
		BodyOffset.Union()
	);
	Builder.Finish(SendMsgData);
	SendFlatBufferMessage(Builder);
}

void ULoginClientSubsystem::SendPlayerListRequest()
{
	flatbuffers::FlatBufferBuilder Builder;
	auto BodyOffset = LoginProtocol::CreateC2S_PlayerListRequest(Builder);
	auto SendMsgData = LoginProtocol::CreateMessageEnvelope(
		Builder,
		GetTimeStamp(),
		LoginProtocol::Payload::C2S_PlayerListRequest,
		BodyOffset.Union()
	);
	Builder.Finish(SendMsgData);
	SendFlatBufferMessage(Builder);
}

void ULoginClientSubsystem::SendPlayerReadyRequest(const FString& SessionToken, const bool bIsReady)
{
	std::string TokenCharBuf = TCHAR_TO_UTF8(*SessionToken);

	flatbuffers::FlatBufferBuilder Builder;
	auto TokenOffset = Builder.CreateString(TokenCharBuf);
	auto BodyOffset = LoginProtocol::CreateC2S_GameReadyRequest(Builder,
		TokenOffset,
		bIsReady
	);
	auto SendMsgData = LoginProtocol::CreateMessageEnvelope(
		Builder,
		GetTimeStamp(),
		LoginProtocol::Payload::C2S_GameReadyRequest,
		BodyOffset.Union()
	);
	Builder.Finish(SendMsgData);
	SendFlatBufferMessage(Builder);
}
