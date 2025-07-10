#pragma once

#include <iostream>
#include <objbase.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace util
{

class UuidHelper
{
public:
    // UUID를 문자열로 변환
    static std::string UuidToString(const UUID& uuid)
    {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');

        ss << std::setw(8) << uuid.Data1 << "-";
        ss << std::setw(4) << uuid.Data2 << "-";
        ss << std::setw(4) << uuid.Data3 << "-";

        ss << std::setw(2) << static_cast<int>(uuid.Data4[0]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[1]) << "-";
        ss << std::setw(2) << static_cast<int>(uuid.Data4[2]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[3]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[4]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[5]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[6]);
        ss << std::setw(2) << static_cast<int>(uuid.Data4[7]);

        return ss.str();
    }

    // UUID 생성
    static std::string GenerateSessionToken()
    {
        UUID uuid;
        // UUID 생성. 성공 시 RPC_S_OK 반환
        RPC_STATUS status = UuidCreate(&uuid);

        // 생성 실패 에러
        if (status != RPC_S_OK)
        {
            std::cout << "[ERROR] Failed to Generate SessionToken" << std::endl;
            return "";
        }

        return UuidToString(uuid);
    }


};

} // namespace util
