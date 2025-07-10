// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WyvernCharacter.h"
#include "MonsterInterface.h"
#include "../PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/MyHUD.h"

AMyMonAIController::AMyMonAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SetPerceptionComponent(*AIPerception);

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	UAISenseConfig_Touch* TouchConfig = CreateDefaultSubobject<UAISenseConfig_Touch>(TEXT("Touch_Config"));

	SightConfig->SightRadius = 3000.0f;
	SightConfig->LoseSightRadius = 3500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 70.0f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;


	TouchConfig->DetectionByAffiliation.bDetectEnemies = true;
	TouchConfig->DetectionByAffiliation.bDetectNeutrals = true;
	TouchConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*TouchConfig);
}

void AMyMonAIController::BeginPlay()
{
	Super::BeginPlay();

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,
		&AMyMonAIController::ProcessPerceptionUpdated);
}

void AMyMonAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HasAuthority())
	{
		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(InPawn);

		if (WyvernCharacter)
		{
			if (WyvernCharacter->WyvernBehaviorTree)
			{
				RunBehaviorTree(WyvernCharacter->WyvernBehaviorTree);
			}
		}
	}
}

void AMyMonAIController::OnUnPossess()
{
	Super::OnUnPossess();

	Destroy();
}

void AMyMonAIController::ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (HasAuthority())
	{
		APlayerCharacter* PlayerCharacter = Cast< APlayerCharacter>(Actor);

		if (PlayerCharacter)
		{
			AddTargetActor(Actor);
		}
	}
	
}

void AMyMonAIController::AddTargetActor(AActor* InTarget)
{
	IMonsterInterface* Monster = Cast<IMonsterInterface>(GetPawn());
	if (Monster)
	{
		if (Monster->AddTargetActor(InTarget))
		{
			UBlackboardComponent* BB = GetBrainComponent()->GetBlackboardComponent();
			if (BB->GetValueAsEnum("MonAIState") == (uint8)EAIState::Patrol)
			{
				BB->SetValueAsObject("TargetActor", InTarget);
				BB->SetValueAsEnum("MonAIState", (uint8)EAIState::Chase);
			}
		}
	}
}

void AMyMonAIController::RemoveTargetActor(AActor* InTarget)
{
	IMonsterInterface* Monster = Cast<IMonsterInterface>(GetPawn());
	if (Monster)
	{
		if (Monster->RemoveTargetActor(InTarget))
		{
			AActor* CurrentTarget = Cast<AActor>(GetBrainComponent()->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
			if (CurrentTarget && CurrentTarget == InTarget)
			{
				ChangeTargetActor();
			}
		}
	}
}

void AMyMonAIController::ChangeTargetActor()
{
	IMonsterInterface* Monster = Cast<IMonsterInterface>(GetPawn());
	if (Monster)
	{
		AActor* ChangedTarget = Monster->ChangeTargetActor();
		if (ChangedTarget)
		{
			UBlackboardComponent* BB = GetBrainComponent()->GetBlackboardComponent();
			BB->SetValueAsObject("TargetActor", ChangedTarget);
		}
	}
}

void AMyMonAIController::CheckTargetActors()
{
	IMonsterInterface* Monster = Cast<IMonsterInterface>(GetPawn());
	if (Monster)
	{
		Monster->CheckTargetActors();
	}
}
