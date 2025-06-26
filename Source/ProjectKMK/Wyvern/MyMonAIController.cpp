// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "WyvernCharacter.h"

AMyMonAIController::AMyMonAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SetPerceptionComponent(*AIPerception);

	AIPerception->OnTargetPerceptionUpdated.Add()
}

void AMyMonAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(InPawn);
	if (WyvernCharacter)
	{
		RunBehaviorTree(WyvernCharacter->WyvernBehaviorTree); 
	}
}

void AMyMonAIController::OnUnPossess()
{
	Super::OnUnPossess();


}
