// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKGameModeBase.h"
#include "KMKPlayerController.h"
#include "PlayerCharacter.h"
#include "InGameGameState.h"
#include "Player/RespawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"

AKMKGameModeBase::AKMKGameModeBase()
{
	PlayerControllerClass = AKMKPlayerController::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	GameStateClass = AInGameGameState::StaticClass();
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

void AKMKGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARespawnPoint::StaticClass(), reinterpret_cast<TArray<AActor*>&>(RespawnPoint));
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
