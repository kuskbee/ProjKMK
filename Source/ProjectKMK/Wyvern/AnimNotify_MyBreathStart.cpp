// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MyBreathStart.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "MyBreath.h"
#include "GameFrameWork/Character.h"

void UAnimNotify_MyBreathStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->DoesSocketExist("breath_socket"))
	{
		UE_LOG(LogTemp, Warning, TEXT("No breath_socket exist"));
		return;
	}

	if (Breath)
	{
		FVector SpawnLocation = MeshComp->GetSocketLocation("breath_socket");
		FRotator SpawnRotation = MeshComp->GetSocketRotation("breath_socket");

		AMyBreath* SpawnedBreath = MeshComp->GetWorld()->SpawnActor<AMyBreath>(
			Breath,
			SpawnLocation,
			SpawnRotation
		);

		ACharacter* Instigator = Cast<ACharacter>(MeshComp->GetOwner());
		if (Instigator)
		{
			SpawnedBreath->SetInstigator(Instigator);
		}
	}
}
