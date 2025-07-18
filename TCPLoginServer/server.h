#pragma once

//DB
#include "DB/DatabaseManager.h"

// Winsock, Session Info
#include "PlayerSession.h"

//Packet
#include "flatbuffers/flatbuffers.h"
#include "LoginProtocol_generated.h"

#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

class Server
{
public:
	virtual ~Server();

	bool Initialize();
	void Close();
	void Run();
	
private:
	void HandleClient(SOCKET ClientSocket);

	void ProcessPacket(std::vector<char>& RecvBuf, sql::Connection* ClientConn, SOCKET ClientSocket);

	void ProcessSignUpRequest(const LoginProtocol::MessageEnvelope* MsgEnvelope, sql::Connection* ClientConn, SOCKET ClientSocket);
	void ProcessLoginRequest(const LoginProtocol::MessageEnvelope* MsgEnvelope, sql::Connection* ClientConn, SOCKET ClientSocket);
	void ProcessPlayerListRequest(const LoginProtocol::MessageEnvelope* MsgEnvelope, sql::Connection* ClientConn, SOCKET ClientSocket);
	void ProcessGameReadyRequest(const LoginProtocol::MessageEnvelope* MsgEnvelope, sql::Connection* ClientConn, SOCKET ClientSocket);
	void BroadcastPacket(flatbuffers::FlatBufferBuilder& Builder, uint8_t BodyType = 0);
	void BroadcastPlayerJoin(PlayerSession& JoinPlayer);
	void BroadcastPlayerLeave(PlayerSession& LeavePlayer);
	void BroadcastPlayerChangeState(PlayerSession& ChangePlayer);
	void BroadcastCountdownStartGame(const bool bIsStart, const int32_t RemainSeconds);
	void BroadcastStartGame();

	bool RecvAll(SOCKET sock, char* buf, size_t len);
	bool SendAll(SOCKET sock, char* buf, size_t len);
	bool SendFlatBufferMessage(SOCKET Sock, flatbuffers::FlatBufferBuilder& Builder, uint8_t BodyType = 0);
	bool ReceiveFlatBufferMessage(SOCKET Sock, std::vector<char>& RecvBuf, uint32_t& outMessageSize);

	void CleanUpClientSession(SOCKET ClientSocket);

	// Game-logic
	bool IsCanStartCountdown();
	void StartCountdown();
	void StopCountdown();

	SOCKET ListenSocket{ INVALID_SOCKET };
	DatabaseManager DbManager;

	// Key:SessionToken, Value:PlayerSession
	std::unordered_map<std::string, PlayerSession> PlayerSessionMap;

	// Key:PlayerId(DB), Value:SessionToken
	std::unordered_map<int32_t, std::string> TokenByPlayerIdMap;

	// Key:Socket, Value:SessionToken
	std::unordered_map<SOCKET, std::string> TokenBySocketMap;

	mutable std::shared_mutex SessionMutex;

	std::atomic<bool> bIsCountdownRunning = false; // 10초 카운트다운 진행 여부
	std::shared_ptr<std::thread> CountdownThread;

	const int32_t CountdownSeconds = 10;

	std::string DediServerIp = "kusk.iptime.org";
	//std::string DediServerIp = "127.0.0.1";
	unsigned short DediPort = 31684;

	unsigned short ServerPort = 29825;
};