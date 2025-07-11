// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyDefine.h"
#include "MyMonAIController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_ChangeMonAIState, EAIState, MonAIState);


class UAIPerceptionComponent;
class APlayerCharacter;

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

	UFUNCTION()
	void ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void AddTargetActor(AActor* InTarget);

	UFUNCTION()
	void RemoveTargetActor(AActor* InTarget);

	UFUNCTION()
	void ChangeTargetActor();

	UFUNCTION()
	void CheckTargetActors();

	UFUNCTION()
	void WhenPawnTakeDamage(float KnockBackAnimPlayLength);

	UFUNCTION()
	void RestartBehaviorTree();
};
