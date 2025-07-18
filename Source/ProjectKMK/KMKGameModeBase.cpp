// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKGameModeBase.h"
#include "KMKPlayerController.h"
#include "PlayerCharacter.h"
#include "InGameGameState.h"
#include "Player/RespawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "Player/InGamePlayerState.h"

AKMKGameModeBase::AKMKGameModeBase()
{
	PlayerControllerClass = AKMKPlayerController::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	GameStateClass = AInGameGameState::StaticClass();
	PlayerStateClass = AInGamePlayerState::StaticClass();
}

void AKMKGameModeBase::NotifyPlayerDead(AKMKPlayerController* PlayerControllerThatDied)
{
	if (!HasAuthority()) return;
	if (!PlayerControllerThatDied) return;

	++TeamDeathCount;

	if (AInGameGameState* GS = GetWorld()->GetGameState<AInGameGameState>())
	{
		GS->ReplicatedTeamDeathCount = TeamDeathCount;
	}

	if (TeamDeathCount > MaxTeamDeath)
	{
		if (AInGameGameState* GS = GetWorld()->GetGameState<AInGameGameState>())
		{
			GS->SetCurrentGameState(EGameState::EGS_Lose);
		}
	}
	else
	{
		FTimerHandle RespawnTimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &AKMKGameModeBase::InitiateRespawn, PlayerControllerThatDied);
	
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, TimerDel, RespawnDelay, false);

		RespawnTimer.Add(PlayerControllerThatDied, RespawnTimerHandle);
	}
}

void AKMKGameModeBase::StartCountdown()
{
	UE_LOG(LogTemp, Warning, TEXT("[KMKGameMode::StartCountdown]"));
	CurrentCountdown = RestartCountdown + 1;

	GetWorldTimerManager().SetTimer(RestartCountdownTimerHandle, this, &AKMKGameModeBase::RestartCountdownTick, 1.0f, true, 0.f);
}

void AKMKGameModeBase::RestartCountdownTick()
{
	CurrentCountdown--;

	UE_LOG(LogTemp, Warning, TEXT("[KMKGameMode::RestartCountdownTick] %d"), CurrentCountdown);

	if (AInGameGameState* GS = GetWorld()->GetGameState<AInGameGameState>())
	{
		GS->RestartCountDown = CurrentCountdown;
	}

	if (CurrentCountdown == 0)
	{
		StopCountdown();

		OnCountdownFinished();
	}
}

void AKMKGameModeBase::OnCountdownFinished()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto PC = Cast<APlayerController>(It->Get());
		if (IsValid(PC) && !RestartClientLevelName.IsEmpty())
		{
			PC->ClientTravel(RestartClientLevelName, TRAVEL_Absolute);
		}
	}

	// 약간의 여유를 두고(클라이언트가 나가는 틈) 서버만 새 맵으로 갈 수 있게 타이머 예약
	GetWorldTimerManager().SetTimer(
		LevelTravelHandle,
		[this]()
		{
			// 전용 서버 모드일 때만 실행
			if (GetNetMode() == NM_DedicatedServer)
			{
				// 맵이름?listen 으로 실행하면 다시 리슨 모드로 기동
				FString URL = FString::Printf(TEXT("%s?listen"), *RestartServerLevelName);
				GetWorld()->ServerTravel(URL);
			}
		},
		1.0f,
		false
	);
}

void AKMKGameModeBase::StopCountdown()
{
	if (GetWorldTimerManager().IsTimerActive(RestartCountdownTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(RestartCountdownTimerHandle);
	}
}

void AKMKGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARespawnPoint::StaticClass(), reinterpret_cast<TArray<AActor*>&>(RespawnPoint));
}

//void AKMKGameModeBase::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	if (AInGameGameState* GS = GetGameState<AInGameGameState>())
//	{
//		GS->Multicast_PlayerJoined(Cast<AInGamePlayerState>(NewPlayer->PlayerState));
//	}
//}
//
void AKMKGameModeBase::Logout(AController* Exiting)
{
	UE_LOG(LogTemp, Warning, TEXT("[AKMKGameModeBase] Logout"));
	if (Exiting && Exiting->PlayerState)
	{
	    // 이 로그가 출력되면, 아래 줄에서 출력되는 PlayerState의 전체 이름을 복사해야 합니다.
	     UE_LOG(LogTemp, Warning, TEXT("[AKMKGameModeBase] Logout ========================================================="));
	     UE_LOG(LogTemp, Warning, TEXT("[AKMKGameModeBase] Logout INVESTIGATING: PlayerState about to	be destroyed."));
	     UE_LOG(LogTemp, Warning, TEXT("[AKMKGameModeBase] Logout Full Name to copy: %s"), *Exiting->PlayerState->GetFullName());
	     UE_LOG(LogTemp, Warning, TEXT("[AKMKGameModeBase] Logout ========================================================"));
	}
	Super::Logout(Exiting);
}

ARespawnPoint* AKMKGameModeBase::GetAvailableRespawnPoint()
{
	for (ARespawnPoint* Point : RespawnPoint)
	{
		if (IsValid(Point) && !Point->bIsOccupied)
		{
			Point->SetOccupied(true);
			return Point;
		}
	}
	return RespawnPoint.IsValidIndex(0) ? RespawnPoint[0] : nullptr;
}

ARespawnPoint* AKMKGameModeBase::GetAvailableRespawnPoint(AKMKPlayerController* KMKPlayerController)
{
	return GetAvailableRespawnPoint();
}

void AKMKGameModeBase::InitiateRespawn(AKMKPlayerController* PlayerControllerToRespawn)
{
	if (!HasAuthority() || !PlayerControllerToRespawn) return;

	APlayerCharacter* CharacterToRespawn = Cast<APlayerCharacter>(PlayerControllerToRespawn->GetPawn());
	if (!CharacterToRespawn) return;

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	ARespawnPoint* PointToSpawn = GetAvailableRespawnPoint(PlayerControllerToRespawn);

	if (PointToSpawn && IsValid(PointToSpawn))
	{
		SpawnLocation = PointToSpawn->GetActorLocation();
		SpawnRotation = PointToSpawn->GetActorRotation();
	}
	else
	{
		AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(PlayerControllerToRespawn);
		
		if (PlayerStart)
		{
			SpawnLocation = PlayerStart->GetActorLocation();
			SpawnRotation = PlayerStart->GetActorRotation();
		}
		else
		{
			SpawnLocation = CharacterToRespawn->GetActorLocation();
			SpawnRotation = CharacterToRespawn->GetActorRotation();
		}
	}
	
	CharacterToRespawn->Multicast_RespawnCharacter(SpawnLocation, SpawnRotation);

	if (PointToSpawn && IsValid(PointToSpawn))
	{
		PointToSpawn->SetOccupied(false);
	}

	RespawnTimer.Remove(PlayerControllerToRespawn);
}
