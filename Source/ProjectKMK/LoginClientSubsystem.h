// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "flatbuffers/flatbuffers.h"
#include "LoginProtocol_generated.h"
#include "NetworkTypeDefine.h"
#include "PlayerInfo.h"
#include "Containers/Queue.h"

#include <thread>
#include "LoginClientSubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoginResponse, ELoginServerErrorCode, ErrCode, FString, Nickname, FString, SessionToken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignUpResponse, ELoginServerErrorCode, ErrCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerListResponse, const TArray<FPlayerInfo>&, PlayerInfos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerInOutLobby, FPlayerInfo, PlayerInfo, bool, bIsJoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerGameReady, FString, UserId, EEntryState, CurrentState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCountdownStartGame, bool, bIsStart, int, RemainSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStartGame, FString, DediIp, int, DediPort);

UCLASS()
class PROJECTKMK_API ULoginClientSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "TCP")
	void ConnectToServer(const FString& HostOrIp = "127.0.0.1", int32 Port = 29825);

	UFUNCTION(BlueprintCallable, Category = "TCP")
	void DisconnectFromServer();

	bool IsConnect();

	UFUNCTION(BlueprintCallable)
	void SendLoginRequest(const FString& UserId, const FString& Password);

	UFUNCTION(BlueprintCallable)
	void SendSignUpRequest(const FString& UserId, const FString& Password, const FString& Nickname);

	UFUNCTION(BlueprintCallable)
	void SendPlayerListRequest();

	UFUNCTION(BlueprintCallable)
	void SendPlayerReadyRequest(const FString& SessionToken, const bool bIsReady);

private:
	void NetworkPolling();
	void RecvLoop();

	bool SendFlatBufferMessage(flatbuffers::FlatBufferBuilder& Builder);
	bool ReceiveFlatBufferMessage(TArray<uint8_t>& RecvBuf, uint32_t& outMessageSize);
	bool RecvAllBlocking(TArray<uint8_t>& RecvBuf, int32 RecvBufLen);

	void ProcessPacket(TArray<uint8_t>& RecvBuf);

	void ProcessLoginResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessSignUpResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessPlayerListResponse(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessPlayerInOutLobby(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessPlayerChangeState(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessCountdownStartGame(const LoginProtocol::MessageEnvelope* MsgEnvelope);
	void ProcessStartGame(const LoginProtocol::MessageEnvelope* MsgEnvelope);

	TObjectPtr<FSocket> Socket;
	FTimerHandle NetworkTimerHandle;

public:

	UPROPERTY(BlueprintAssignable)
	FOnLoginResponse OnLoginResponseDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSignUpResponse OnSignUpResponseDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerListResponse OnPlayerListResponseDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerInOutLobby OnPlayerInOutLobbyDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerGameReady OnPlayerChangeStateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnCountdownStartGame OnCountdownStartGameDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnStartGame OnStartGameDelegate;

	TUniquePtr<std::thread> LoginRecvThread;
	std::atomic<bool> bRunRecv{ false };

	// Spsc : Single Producer, Single Consumer
	TQueue<TArray<uint8>, EQueueMode::Spsc> RecvPackets;
};
