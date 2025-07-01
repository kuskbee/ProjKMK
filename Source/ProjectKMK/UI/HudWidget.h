// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWidget.generated.h"

//class UButton;
class UImage;
class UProgressBar;

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	/*UPROPERTY(meta=(BindWidgetOptional), VisibleAnywhere, Category="UI", BlueprintReadOnly)
	TObjectPtr<UButton> ButtonStart;*/

	UPROPERTY(meta = (BindWidgetOptional), VisibleAnywhere, Category = "UI", BlueprintReadOnly)
	TObjectPtr<UImage> GameLoseUI;

	UPROPERTY(meta = (BindWidgetOptional), VisibleAnywhere, Category = "UI", BlueprintReadOnly)
	TObjectPtr<UImage> GameWinUI;

	UPROPERTY(meta = (BindWidgetOptional), VisibleAnywhere, Category = "UI", BlueprintReadOnly)
	TObjectPtr<UProgressBar> MonsterHealthBar;

	UPROPERTY(meta = (BindWidgetOptional), VisibleAnywhere, Category = "UI", BlueprintReadOnly)
	TObjectPtr<UProgressBar> PlayerHealthBar;

	void DoGameWin();

	void DoGameOver();
	
	void ShowMonsterHealthBar(bool bIsShowHealthBar);

	void SetMonsterHpPercent(float CurHP, float MaxHP);
	
	void SetPlayerHpPercent(float CurHP, float MaxHP);

};
