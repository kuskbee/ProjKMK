#pragma once
#include <cstdint>

enum class SignUpStatus : uint8_t
{
	Success,
	DuplicateUserId,
	DuplicateNickname,
	OtherError
};

enum class LoginStatus : uint8_t
{
	Success,
	UserIdNotFound,
	InvalidPassword,
	OtherError
};