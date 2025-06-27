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

AMyMonAIController::AMyMonAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SetPerceptionComponent(*AIPerception);

	//Perception Settings

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

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, 
		&AMyMonAIController::ProcessPerceptionUpdated);

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


}

void AMyMonAIController::ProcessPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APlayerCharacter* PlayerCharacter = Cast< APlayerCharacter>(Actor);
	if (PlayerCharacter)
	{
		ChasePlayer(PlayerCharacter);
	}

}

void AMyMonAIController::ChasePlayer(AActor* Actor)
{
	AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(K2_GetPawn());
	if (WyvernCharacter)
	{
		if (WyvernCharacter->MonAIState != EAIState::Battle)
		{
			BrainComponent->GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
			BrainComponent->GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8 (EAIState::Chase));
			WyvernCharacter->MonAIState = EAIState::Chase;

			if (WyvernCharacter->Phase == EPhase::ThirdPhase)
			{
				ShowMonsterHealthBar();
			}
		}
	}

}


// Need MyHud class Please HanJoon NIM;
void AMyMonAIController::ShowMonsterHealthBar()
{
	//AMyHUD* MyHud = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	//if (MyHud)
	//{
	//	if (!MyHud->IsShowHealthBar)
	//	{
	//		MyHud->ShowMonsterHealthBar();
	//		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(K2_GetPawn());
	//		if (WyvernCharacter)
	//		{
	//			MyHud->BindWyvernEvent(WyvernCharacter);
	//		}
	//	}
	//}
	
}
