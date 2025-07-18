#pragma once

#include "CoreMinimal.h"
#include "NetworkTypeDefine.h"
#include "PlayerInfo.generated.h"


USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	FPlayerInfo()
		: UserId(TEXT(""))
		, Nickname(TEXT(""))
		, CurrentState(EEntryState::EPS_Lobby)
	{
	}

	FPlayerInfo(const FString& InId, const FString& InName, EEntryState InState)
		: UserId(InId)
		, Nickname(InName)
		, CurrentState(InState)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FString Nickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	EEntryState CurrentState;


};
