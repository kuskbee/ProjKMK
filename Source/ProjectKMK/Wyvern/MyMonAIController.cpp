// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"
#include "WyvernCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	GetPerceptionComponent()->OnTargetPerceptionForgotten.AddDynamic(this, 
		&AMyMonAIController::ProcessPerceptionForgetUpdated);
}

void AMyMonAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(InPawn);

	if (WyvernCharacter)
	{
		if (WyvernCharacter->WyvernBehaviorTree)
		{
			RunBehaviorTree(WyvernCharacter->WyvernBehaviorTree);
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
	APlayerCharacter* PlayerCharacter = Cast< APlayerCharacter>(Actor);

	if (PlayerCharacter)
	{
		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(GetPawn());

		if (WyvernCharacter && WyvernCharacter->MonAIState == EAIState::Patrol)
		{
			BrainComponent->GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
			BrainComponent->GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Chase));
			WyvernCharacter->MonAIState = EAIState::Chase;

			if (WyvernCharacter->Phase == EPhase::ThirdPhase)
			{
				ShowMonsterHealthBar();
			}
		}
	}
}

void AMyMonAIController::ProcessPerceptionForgetUpdated(AActor* Actor)
{
	//AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(Actor);
	//if (WyvernCharacter)
	//{
	//	WyvernCharacter->MonAIState = EAIState::Patrol;
	//	BrainComponent->GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Patrol));
	//}

}

void AMyMonAIController::FirstEncounterTarget(AController* Causer)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Causer->GetCharacter());

	if (Player)
	{
		Players.Add(Player);

		BrainComponent->GetBlackboardComponent()->SetValueAsObject("TargetActor", Player);
		BrainComponent->GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Chase));

		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(GetPawn());

		if (WyvernCharacter)
		{
			WyvernCharacter->MonAIState = EAIState::Chase;
		}
	}
}

void AMyMonAIController::ShowMonsterHealthBar()
{
	AMyHUD* LocalHud = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	if (LocalHud)
	{
		if (!LocalHud->IsShowHealthBar)
		{
			LocalHud->ShowMonsterHealthBar();
			AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(K2_GetPawn());
			if (WyvernCharacter)
			{
				LocalHud->BindWyvernEvent(WyvernCharacter);
			}
		}
	}
}
