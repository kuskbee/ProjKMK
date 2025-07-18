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
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/MyHUD.h"
#include "TimerManager.h"

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

	StopMovement();

	Destroy();
}

void AMyMonAIController::ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (HasAuthority())
	{
		APlayerCharacter* PlayerCharacter = Cast< APlayerCharacter>(Actor);

		if (PlayerCharacter && PlayerCharacter->EchoState != EPlayerState::EPS_Dead)
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
		Monster->RemoveTargetActor(InTarget);
	}
}

void AMyMonAIController::ChangeTargetActor()
{
	IMonsterInterface* Monster = Cast<IMonsterInterface>(GetPawn());
	if (Monster)
	{
		AActor* ChangedTarget = Monster->ChangeTargetActor();
		UBlackboardComponent* BB = GetBrainComponent()->GetBlackboardComponent();
		if (ChangedTarget)
		{
			BB->SetValueAsObject("TargetActor", ChangedTarget);
		}
		else
		{
			BB->SetValueAsEnum("MonAIState", (uint8)EAIState::Patrol);
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


void AMyMonAIController::RestartBehaviorTree()
{
	ACharacter* MonChar = Cast<ACharacter>(GetPawn());
	if (MonChar)
	{
		MonChar->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}

	UBehaviorTreeComponent* OwnedComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (OwnedComp)
	{
		OwnedComp->ResumeLogic("Resumed after pause");
	}
}

void AMyMonAIController::WhenPawnTakeDamage(float KnockBackAnimPlayLength)
{
	StopMovement();

	UBehaviorTreeComponent* OwnedComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (OwnedComp)
	{
		OwnedComp->PauseLogic("Paused by AIController");
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AMyMonAIController::RestartBehaviorTree,
		KnockBackAnimPlayLength,
		false
	);
}
