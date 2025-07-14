// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "UI/MyHUD.h"
#include "KMKGameModeBase.h"

AInGameGameState::AInGameGameState()
{
	bReplicates = true;
	CurrentGameState = EGameState::EGS_None;
	ReplicatedTeamDeathCount = 0;
}

void AInGameGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnRep_ReplicatedTeamDeathCount();

		OnTeamDeathCountChanged.Broadcast(3);
	}

	//게임 시작 시 초기 상태 설정(현재 Ready 상태 - 주석처리)
	/*if (HasAuthority())
	{
		SetCurrentGameState(EGameState::EGS_Ready);
	}*/

}

void AInGameGameState::OnRep_CurrentGameState()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
		{
			LocalHud->EventChangeGameState(CurrentGameState);
		}
	}

	OnGameStateChanged.Broadcast(CurrentGameState);
}

void AInGameGameState::SetCurrentGameState(EGameState NewState)
{
	//서버에서만 상태 변경될 때만 호출
	if (HasAuthority() && CurrentGameState != NewState)
	{
		//서버에서 상태 변경
		CurrentGameState = NewState;

		//Listen Server 용도
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
			{
				LocalHud->EventChangeGameState(CurrentGameState);
			}
		}//Dedicated Server 사용 시 주석(삭제)처리할 것

		OnGameStateChanged.Broadcast(CurrentGameState);

		// 재시작 로직
		if (CurrentGameState == EGameState::EGS_Lose ||
			CurrentGameState == EGameState::EGS_Win)
		{
			if (AKMKGameModeBase* GM = GetWorld()->GetAuthGameMode<AKMKGameModeBase>())
			{
				if (CurrentGameState == EGameState::EGS_Win) {
					GM->SetRestartCountdown(15);
				}
				GM->StartCountdown();
			}
		}
	}
}

void AInGameGameState::OnRep_ReplicatedTeamDeathCount()
{
	const int32 RemainingLives = FMath::Max(0, 3 - ReplicatedTeamDeathCount);

	//HUD & UI 팀 데스 카운트 업데이트 시 로직 추가
	UE_LOG(LogTemp, Warning, TEXT("[AInGameGameState::OnRep_ReplicatedTeamDeathCount] DeathCount: %d, RemainingLives: %d"), ReplicatedTeamDeathCount, RemainingLives);
	OnTeamDeathCountChanged.Broadcast(RemainingLives);
}

void AInGameGameState::OnRep_RestartCountdown()
{
	UE_LOG(LogTemp, Warning, TEXT("[AInGameGameState::OnRep_RestartCountdown] %d"), RestartCountDown);
	OnRestartCountdownChanged.Broadcast(RestartCountDown);
}

void AInGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInGameGameState, CurrentGameState);
	DOREPLIFETIME(AInGameGameState, ReplicatedTeamDeathCount);
	DOREPLIFETIME(AInGameGameState, RestartCountDown);
}
