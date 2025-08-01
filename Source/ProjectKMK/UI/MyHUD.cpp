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
#include "../InGameGameState.h"
#include "../Player/InGamePlayerState.h"

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
		
			BindGameStateEvent();
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

		StatusComponent->OnDead.AddDynamic(this, &AMyHUD::OnPlayerDaed);
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

void AMyHUD::BindGameStateEvent()
{
	if (AInGameGameState* GS = GetWorld()->GetGameState<AInGameGameState>())
	{
		GS->OnGameStateChanged.AddDynamic(this, &AMyHUD::OnGameStateChanged);
		GS->OnRestartCountdownChanged.AddDynamic(this, &AMyHUD::OnRestartCountdownChanged);
		GS->OnTeamDeathCountChanged.AddDynamic(this, &AMyHUD::OnRemainingLives);
	
		const int32 RemainingLives = FMath::Max(0, 3 - GS->ReplicatedTeamDeathCount);

		UE_LOG(LogTemp, Warning, TEXT("[MyHUD::BindGameStateEvent] ReplicatedTeamDeathCount: %d, Initial RemainingLives: %d"), GS->ReplicatedTeamDeathCount, RemainingLives);

		OnRemainingLives(RemainingLives);
	}

}

void AMyHUD::BindPlayerStateEvent(AInGamePlayerState* PS)
{
	UE_LOG(LogTemp, Warning, TEXT("[AMyHUD] BindPlayerStateEvent"));
	if (IsValid(PS))
	{
		PS->OnDestroyed.AddDynamic(this, &AMyHUD::OnDestroyedPlayerState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AMyHUD] BindPlayerStateEvent PlayerState is Invalid!!"));
	}
}

void AMyHUD::OnGameStateChanged(EGameState NewState)
{
	EventChangeGameState(NewState);
}

void AMyHUD::OnPlayerDaed(bool bDead)
{
}

void AMyHUD::OnRemainingLives(int32 RemainingLives)
{
	UE_LOG(LogTemp, Warning, TEXT("[MyHUD::OnRemainingLives] Displaying RemainingLives on HUD: %d"), RemainingLives);


	if (IsValid(HudWidget))
	{
		HudWidget->SetDeathCount(RemainingLives);
	}
}

void AMyHUD::OnRestartCountdownChanged(int32 RestartCountdown)
{
	if (IsValid(HudWidget))
	{
		HudWidget->SetRestartCountdown(RestartCountdown);
	}
}
