// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WyvernCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMotionWarpingComponent;
class UMonStateComponent;

UCLASS()
class PROJECTKMK_API AWyvernCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWyvernCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TObjectPtr<UMonStateComponent> MonStateComponent;

};
