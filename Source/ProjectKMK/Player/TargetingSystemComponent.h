// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"

class UCameraComponent;

// Event Dispatcher Define
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnNewTargetSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetLockedOnTargetSignature);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTKMK_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(ACharacter* Owner, UCameraComponent* CameraComponent);
	TArray<AActor*> FindAllTargets();
	AActor* LockOnToTarget();
	void ResetLockedOnTarget();
	void ChangeTarget();

public:
	UPROPERTY(VisibleAnywhere, Category = "Reference", BlueprintReadOnly)
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, Category = "Reference", BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TraceDistance = 10000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AActor>> TargetEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> LockedOnTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RadiusToLockOn = 10000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int LockedOnTargetIndex;

	UPROPERTY(EditAnywhere)
	FString TargetTag = TEXT("Enemy");

	FOnLockOnNewTargetSignature OnLockOnNewTarget;
	FOnResetLockedOnTargetSignature OnResetLockedOnTarget;
};
