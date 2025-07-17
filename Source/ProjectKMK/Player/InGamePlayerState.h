// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "InGamePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetUserNickname, FString, Nickname);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, AInGamePlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, AInGamePlayerState*, PlayerState);

UCLASS()
class PROJECTKMK_API AInGamePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;

public:
	virtual void BeginPlay() override;
	void BindEventToHUD();
	virtual void Destroyed() override;

	UFUNCTION()
	void OnRep_Nickname();

	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& Nickname);
	void ServerSetNickname_Implementation(const FString& Nickname);

	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_PlayerJoined(AInGamePlayerState* NewPS);
	//void Multicast_PlayerJoined_Implementation(AInGamePlayerState* NewPS);

	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_PlayerLeft(AInGamePlayerState* LeftPS);
	//void Multicast_PlayerLeft_Implementation(AInGamePlayerState* LeftPS);

public:
	UPROPERTY(ReplicatedUsing = OnRep_Nickname, BlueprintReadOnly)
	FString UserNickname = TEXT("Nickname");

	UPROPERTY(BlueprintAssignable)
	FOnSetUserNickname OnSetUserNicknameDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoinedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLeft  OnPlayerLeftDelegate;
};
