// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MyBreathStart.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "MyBreath.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Breath =  %s"),
			*Breath->GetDisplayNameText().ToString());
		FVector SpawnLocation =
			MeshComp->GetSocketLocation("breath_socket");
		FRotator SpawnRotation =
			MeshComp->GetSocketRotation("breath_socket");

		//// 이거 AnimNotify 밖에서 실행시켜야 안터짐
		//GetWorld()->SpawnActor<AMyBreath>(
		//	Breath,
		//	SpawnLocation,
		//	SpawnRotation
		//);
	}
}
