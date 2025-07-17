// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "InGamePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetUserNickname, FString, Nickname);

UCLASS()
class PROJECTKMK_API AInGamePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;

public:
	virtual void BeginPlay() override;
	void BindEventToHUD();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_Nickname();

	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& Nickname);
	void ServerSetNickname_Implementation(const FString& Nickname);

public:
	UPROPERTY(ReplicatedUsing = OnRep_Nickname, BlueprintReadOnly)
	FString UserNickname = TEXT("Nickname");

	UPROPERTY(BlueprintAssignable)
	FOnSetUserNickname OnSetUserNicknameDelegate;
};
