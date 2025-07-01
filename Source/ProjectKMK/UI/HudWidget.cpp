// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void UHudWidget::DoGameWin()
{
	GameWinUI->SetVisibility(ESlateVisibility::Visible);

	PlayerHealthBar->SetVisibility(ESlateVisibility::Collapsed);
	MonsterHealthBar->SetVisibility(ESlateVisibility::Collapsed);
}

void UHudWidget::DoGameOver()
{
	GameLoseUI->SetVisibility(ESlateVisibility::Visible);
}

void UHudWidget::ShowMonsterHealthBar(bool bIsShowHealthBar)
{
	MonsterHealthBar->SetVisibility(ESlateVisibility::Visible);
}

void UHudWidget::SetMonsterHpPercent(float CurHP, float MaxHP)
{
	if (MonsterHealthBar && MaxHP > 0.0f)
	{
		float Percent = CurHP / MaxHP;
		MonsterHealthBar->SetPercent(Percent);
	}
}

void UHudWidget::SetPlayerHpPercent(float CurHP, float MaxHP)
{
	if (PlayerHealthBar && MaxHP > 0.0f)
	{
		float Percent = CurHP / MaxHP;
		PlayerHealthBar->SetPercent(Percent);
	}
}
