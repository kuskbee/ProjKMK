// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "UI/MyHUD.h"
#include "StatusComponent.h"
#include "GameFramework/HUD.h"

AKMKPlayerController::AKMKPlayerController()
{
}

void AKMKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);		
}

void AKMKPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AKMKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* ControlledPawn = GetPawn())
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ControlledPawn))
		{
			if (UStatusComponent* StatusComponent = PlayerCharacter->StatusComponent)
			{
				if (AMyHUD* LocalHUD = Cast<AMyHUD>(GetHUD()))
				{
					LocalHUD->BindPlayerEvent(StatusComponent);

					StatusComponent->UpdateUIHp();
				}
			}
		}
	}
}

void AKMKPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (IsLocalController())
	{
		SetupEnhanceInput();
	}
}

void AKMKPlayerController::SetupEnhanceInput()
{
	APlayerCharacter* MyPawn = Cast<APlayerCharacter>(GetPawn());

	if (IsValid(MyPawn))
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (MyPawn->IMC_Default)
				{
					InputSystem->ClearAllMappings();
					InputSystem->AddMappingContext(MyPawn->IMC_Default, 0);
				}
			}
		}
	}
}
