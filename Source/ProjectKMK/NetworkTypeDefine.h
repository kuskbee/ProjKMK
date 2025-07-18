#pragma once

UENUM(BlueprintType)
enum class ELoginServerErrorCode : uint8
{
	ELS_Success UMETA(DisplayName = "Success"),
	ELS_Login_Fail_UserNotFound  UMETA(DisplayName = "Login_Fail_UserNotFound"),
	ELS_Login_Fail_InvalidPassword UMETA(DisplayName = "Login_Fail_InvalidPassword"),
	ELS_SignUp_Fail_UsernameExists UMETA(DisplayName = "SignUp_Fail_UsernameExists"),
	ELS_SignUp_Fail_NicknameExists UMETA(DisplayName = "SignUp_Fail_NicknameExists"),
	ELS_Auth_Fail_InvalidToken UMETA(DisplayName = "Auth_Fail_InvalidToken"),
	ELS_InvalidRequest UMETA(DisplayName = "InvalidRequest"),
	ELS_ServerError UMETA(DisplayName = "ServerError"),
};

UENUM(BlueprintType)
enum class EEntryState : uint8
{
	EPS_Lobby UMETA(DisplayName = "Lobby"),     // 로비에 입장한 상태
	EPS_Ready UMETA(DisplayName = "Ready"),     // 로비에서 준비 완료한 상태
	EPS_InGame UMETA(DisplayName = "InGame")     // 게임에 입장한 상태
};
