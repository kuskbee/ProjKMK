#pragma once
#include <chrono>
#include <cstdint>

uint64_t GetTimeStamp()
{
	// 시스템 시계를 UTC epoch(1970-01-01) 기준으로 읽어서
	// 밀리초 단위 정수로 변환
	using namespace std::chrono;
	auto now = system_clock::now();
	auto ms = duration_cast<milliseconds>(now.time_since_epoch());
	return static_cast<uint64_t>(ms.count());
}
