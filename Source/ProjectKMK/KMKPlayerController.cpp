// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"

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
}
