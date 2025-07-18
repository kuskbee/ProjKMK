// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UITypes.h"
#include "MyHUD.generated.h"

class UHudWidget;
class UStatusComponent;
class AInGamePlayerState;

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	void BindWyvernEvent(ACharacter* WyvernCharacter);

	void EventChangeGameState(EGameState GameState);

	UFUNCTION()
	void EventMonsterUpdateHP(float MaxHP, float WeaknessHP, float CurHP, float CurWeakHP);

	void ShowMonsterHealthBar();

	void BindPlayerEvent(UStatusComponent* StatusComponent);

	UFUNCTION()
	void EventPlayerUpdateHP(float CurHP, float MaxHP);

	void BindGameStateEvent();

	void BindPlayerStateEvent(AInGamePlayerState* PS);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHudWidget> HudWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsShowHealthBar;

	FTimerHandle DoGameWinTimerHandle;

	UFUNCTION()
	void OnGameStateChanged(EGameState NewState);

	UFUNCTION()
	void OnPlayerDaed(bool bDead);

	UFUNCTION()
	void OnRemainingLives(int32 RemainingLives);

	UFUNCTION()
	void OnRestartCountdownChanged(int32 RestartCountdown);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroyedPlayerState(AActor* DestroyedActor);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerListUI();
};
