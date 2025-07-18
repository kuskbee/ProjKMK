// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MyBreathStart.generated.h"

/**
 * 
 */

class AMyBreath;

UCLASS()
class PROJECTKMK_API UAnimNotify_MyBreathStart : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	TSubclassOf<AMyBreath> Breath;
};
