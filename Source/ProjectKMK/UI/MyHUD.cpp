// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "GameFramework/Character.h"
#include "../Wyvern\MonStateComponent.h"
#include "../Wyvern\MyCurMonState.h"
#include "../StatusComponent.h"
#include "HudWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "UITypes.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	FSoftClassPath HudWidgetPath(TEXT("/UiPlugin/CRPlugIn/Content/BluePrints/Widget/InGameHudWidget.InGameHudWidget_C"));

	UClass* WidgetClass = HudWidgetPath.TryLoadClass<UHudWidget>();

	if (WidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetClass is null! Check your path."));
	}

	if (WidgetClass)
	{
		APlayerController* PC = GetOwningPlayerController();

		HudWidget = CreateWidget<UHudWidget>(PC, WidgetClass);
		if (HudWidget)
		{
			HudWidget->AddToViewport();

			PC->bShowMouseCursor = false;

			PC->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HudWidget creation failed."));
		}
	}

}

void AMyHUD::BindWyvernEvent(ACharacter* WyvernCharacter)
{
	if (!WyvernCharacter) return;

	UMonStateComponent* MonStateComponent = WyvernCharacter->FindComponentByClass<UMonStateComponent>();
	if (!MonStateComponent) return;

	MonStateComponent->EventDispatcher_UpdateHP.AddDynamic(this, &AMyHUD::EventMonsterUpdateHP);

	const FMyCurMonState& CurMonState = MonStateComponent->CurMonState;

	EventMonsterUpdateHP(CurMonState.MonData.MaxHP, CurMonState.MonData.WeaknessHP, CurMonState.CurHP, CurMonState.CurWeakHP);
}


void AMyHUD::EventChangeGameState(EGameState GameState)
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(HudWidget, &UHudWidget::DoGameWin);
	switch (GameState)
	{
		case EGameState::EGS_Win:GetWorldTimerManager().SetTimer(DoGameWinTimerHandle, TimerDelegate, 5.0f, false);
			break;
		case EGameState::EGS_Lose:HudWidget->DoGameOver();
			break;
	}
}

void AMyHUD::EventMonsterUpdateHP(float MaxHP, float WeaknessHP, float CurHP, float CurWeakHP)
{
	if (HudWidget)
	{
		HudWidget->SetMonsterHpPercent(CurHP, MaxHP);
	}
}

void AMyHUD::ShowMonsterHealthBar()
{
	if (HudWidget)
	{
		HudWidget->ShowMonsterHealthBar();
	}
	IsShowHealthBar = true;
}

void AMyHUD::BindPlayerEvent(UStatusComponent* StatusComponent)
{
	if (StatusComponent)
	{
		StatusComponent->OnChangeHp.AddDynamic(this, &AMyHUD::EventPlayerUpdateHP);
	}
}

void AMyHUD::EventPlayerUpdateHP(float CurHP, float MaxHP)
{
	//== Convert to Valid Get
	if (IsValid(HudWidget))
	{
		HudWidget->SetPlayerHpPercent(CurHP, MaxHP);
	}
}
