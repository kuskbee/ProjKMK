// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DeathCountText)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UHudWidget::NativeConstruct] DeathCountText is valid"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UHudWidget::NativeConstruct] DeathCountText is NULL! Check UMG binding."));
	}

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

void UHudWidget::SetDeathCount(int32 LifeCount)
{


	if (DeathCountText)
	{

		UE_LOG(LogTemp, Warning, TEXT("[HudWidget::SetDeathCount] HUD will display: %d"), LifeCount);
		DeathCountText->SetText(FText::AsNumber(LifeCount));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HudWidget::SetDeathCount] DeathCount is NULL"));
	}
}
