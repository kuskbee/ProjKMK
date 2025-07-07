// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UI/UITypes.h"
#include "InGameGameState.generated.h"

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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
