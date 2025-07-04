// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Define.h"
#include "MyMonAIController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_ChangeMonAIState, EAIState, MonAIState);


class UAIPerceptionComponent;

UCLASS()
class PROJECTKMK_API AMyMonAIController : public AAIController
{
	GENERATED_BODY()


public:

	AMyMonAIController();
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	EAIState AIState;

	UFUNCTION()
	void ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void ProcessPerceptionForgetUpdated(AActor* Actor);

	UFUNCTION()
	void FindDamageCauser(AController* DamageCauser);

	UFUNCTION()
	void ShowMonsterHealthBar();
};
