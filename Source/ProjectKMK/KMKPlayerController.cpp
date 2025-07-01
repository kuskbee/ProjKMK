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

	UE_LOG(LogTemp, Warning, TEXT("OnPossess"));

	APlayerCharacter* MyPawn = Cast<APlayerCharacter>(InPawn);

	if (IsValid(MyPawn))
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (MyPawn->IMC_Default)
				{
					InputSystem->AddMappingContext(MyPawn->IMC_Default, 0);
				}
			}
		}
	}
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
