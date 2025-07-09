// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WyvernCharacter.h"
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

	K2_DestroyActor();

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
	if (!TargetActors.Contains(InTarget))
	{
		TargetActors.Add(InTarget);

		UBlackboardComponent* BB = GetBrainComponent()->GetBlackboardComponent();
		if (BB->GetValueAsEnum("MonAIState") == (uint8)EAIState::Patrol)
		{
			BB->SetValueAsObject("TargetActor", InTarget);
			BB->SetValueAsEnum("MonAIState", (uint8)EAIState::Chase);
		}
	}
}

void AMyMonAIController::RemoveTargetActor(AActor* InTarget)
{
	if (TargetActors.Contains(InTarget))
	{
		AActor* CurrentTarget = Cast<AActor>(GetBrainComponent()->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (CurrentTarget && CurrentTarget == InTarget)
		{
			ChangeTargetActor();
		}

		TargetActors.Remove(InTarget);
	}
}

void AMyMonAIController::ChangeTargetActor()
{
	if (TargetActors.Num() <= 1)
	{
		return;
	}

	AActor* NewTarget = TargetActors[(int)FMath::RandRange(0, TargetActors.Num() - 1)];

	UBlackboardComponent* BB = GetBrainComponent()->GetBlackboardComponent();

	BB->SetValueAsObject("TargetActor", NewTarget);
	BB->SetValueAsEnum("MonAIState", (uint8)EAIState::Chase);

}

void AMyMonAIController::CheckTargetActors()
{
	for (AActor* Target : TargetActors)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(Target);
		if (Player)
		{
			if (Player->EchoState == EPlayerState::EPS_Dead)
			{
				RemoveTargetActor(Player);
				return CheckTargetActors();
			}
		}
	}
}



void AMyMonAIController::ShowMonsterHealthBar(APlayerCharacter* Player)
{
	APlayerController* OwnerPC = Cast<APlayerController>(Player->GetController());
	if (OwnerPC)
	{
		AMyHUD* LocalHud = Cast<AMyHUD>(OwnerPC->GetHUD());

		if (LocalHud)
		{
			if (!LocalHud->IsShowHealthBar)
			{

				LocalHud->ShowMonsterHealthBar();
				AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(GetPawn());
				if (WyvernCharacter)
				{
					LocalHud->BindWyvernEvent(WyvernCharacter);
				}
			}
		}
	}
}
