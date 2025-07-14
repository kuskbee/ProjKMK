// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UITypes.h"
#include "MyHUD.generated.h"

class UHudWidget;
class UStatusComponent;

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
	void OnRestartCountdownChanged(int32 RestartCountdown);
};
