// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UI/UITypes.h"
#include "InGameGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamDeathCountChanged, int32, CurrentTeamDeathCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRestartCountdownChanged, int32, RestartCountdown);

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AInGameGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AInGameGameState();

	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentGameState, BlueprintReadOnly, Category = "Game State")
	EGameState CurrentGameState;

	UFUNCTION()
	void OnRep_CurrentGameState();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetCurrentGameState(EGameState NewState);

	UPROPERTY(BlueprintAssignable, Category = "Game State")
	FOnGameStateChanged OnGameStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game State")
	FOnTeamDeathCountChanged OnTeamDeathCountChanged;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTeamDeathCount, BlueprintReadOnly, Category = "Game State")
	int32 ReplicatedTeamDeathCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_RestartCountdown)
	int32 RestartCountDown = -1;

	UPROPERTY(BlueprintAssignable)
	FOnRestartCountdownChanged OnRestartCountdownChanged;

	UFUNCTION()
	void OnRep_ReplicatedTeamDeathCount();

	UFUNCTION()
	void OnRep_RestartCountdown();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
