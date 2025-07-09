#pragma once
#include <string>
#include <cstdint>

#include <WinSock2.h>

enum class PlayerState : uint8_t
{
    Lobby,     // 로비에 입장한 상태
    Ready,     // 로비에서 준비 완료한 상태
    InGame     // 게임에 입장한 상태
};

struct PlayerSession
{
    int32_t DbId;                 // DB의 유저 ID
    std::string UserId;           // 유저 ID
    std::string PlayerName;       // 플레이어 닉네임
    SOCKET Socket;              // 해당 클라이언트의 소켓 정보
    PlayerState CurrentState;    // 현재 플레이어 상태 (Lobby, Ready 등)
};