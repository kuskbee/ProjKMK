// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KMKGameModeBase.generated.h"

class AKMKPlayerController;
class ARespawnPoint;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AKMKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AKMKGameModeBase();

	UFUNCTION(BlueprintCallable)
	void NotifyPlayerDead(AKMKPlayerController* PlayerControllerThatDied);

	void StartCountdown();

	UFUNCTION()
	void RestartCountdownTick();

	void OnCountdownFinished();

	void StopCountdown();


protected:

	virtual void BeginPlay() override;

	/*virtual void PostLogin(APlayerController* NewPlayer) override;*/
	//virtual void Logout(AController* Exiting) override;

	ARespawnPoint* GetAvailableRespawnPoint();
	ARespawnPoint* GetAvailableRespawnPoint(AKMKPlayerController* KMKPlayerController);

	void InitiateRespawn(AKMKPlayerController* PlayerControllerToRespawn);

	UPROPERTY()
	int32 TeamDeathCount = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxTeamDeath = 3;

	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 8.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 RestartCountdown = 10.f;

	int32 CurrentCountdown = 0.f;

	UPROPERTY()
	FTimerHandle RestartCountdownTimerHandle;

	FTimerHandle LevelTravelHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RestartServerLevelName = TEXT("/Game/Level/Maps/Map_PhaseNature");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RestartClientLevelName = TEXT("/Game/Level/Maps/LobbyMap");

	UPROPERTY()
	TArray<ARespawnPoint*> RespawnPoint;

	UPROPERTY()
	TMap<AKMKPlayerController*, FTimerHandle> RespawnTimer;
public:
	void SetRestartCountdown(int32 Countdown) { RestartCountdown = Countdown; }

};
