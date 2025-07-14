// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHudWidget::DoGameWin()
{
	GameWinUI->SetVisibility(ESlateVisibility::Visible);

	PlayerHealthBar->SetVisibility(ESlateVisibility::Collapsed);
	MonsterHealthBar->SetVisibility(ESlateVisibility::Collapsed);
	CountdownText->SetVisibility(ESlateVisibility::Visible);
}

void UHudWidget::DoGameOver()
{
	GameLoseUI->SetVisibility(ESlateVisibility::Visible);
	CountdownText->SetVisibility(ESlateVisibility::Visible);
}

void UHudWidget::ShowMonsterHealthBar()
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

void UHudWidget::SetRestartCountdown(int32 RestartCountdown)
{
	if (CountdownText)
	{
		const FString LoadingMent = TEXT("초 뒤 로비로 이동합니다");

		int DotCount = 3 - RestartCountdown % 3;
		CountdownString = FString::Printf(TEXT("%d%s"), RestartCountdown, *LoadingMent);
		for (int i = 0; i < DotCount; ++i)
		{
			CountdownString += TEXT(".");
		}

		CountdownText->SetText(FText::FromString(CountdownString));
	}
}
