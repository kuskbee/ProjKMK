// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KMKGameInstance.generated.h"
//#include "PlayerInfo.h"
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDispatcher_PlayerList);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_AddPlayerOne, FString, Nickname);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_RemovePlayerOne, FString, Nickname);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_ChangeMyReadState, bool bReady);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventDispatcher_PlayerReady, FString, Nickname, bool bIsReady);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_ChangePlayerCnt, int PlayerCnt);


UCLASS()
class PROJECTKMK_API UKMKGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	/*virtual void Init() override;

	void BindLoginClientEvent();

	void OnResponsePlayerList(const TArray<FPlayerInfo>& PlayerInfos);
	void OnPlayerInOutLobby(FPlayerInfo PlayerInfo, bool bIsJoin);
	void OnPlayerChangeState(FString UserId, EEntryState CurrentState);*/

public:
	/*FString Nickname;
	
	FString SessionToken;

	TMap<FString, FPlayerInfo> PlayerList;

	FEventDispatcher_PlayerList EventDispatcher_PlayerList;
	FEventDispatcher_AddPlayerOne EventDispatcher_AddPlayerOne;
	FEventDispatcher_RemovePlayerOne EventDispatcher_RemovePlayerOne;
	FEventDispatcher_ChangeMyReadState EventDispatcher_ChangeMyReadState;
	FEventDispatcher_PlayerReady EventDispatcher_PlayerReady;
	FEventDispatcher_ChangePlayerCnt EventDispatcher_ChangePlayerCnt;
private:
	FTimerHandle RetryBindHandle;*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Login")
	FString Nickname;
};
