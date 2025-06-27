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

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_ChangeMonAIState EventDispatcher_ChangeMonAIState;

	UFUNCTION()
	void ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void ChasePlayer(AActor* Actor);
	void ShowMonsterHealthBar();
};
