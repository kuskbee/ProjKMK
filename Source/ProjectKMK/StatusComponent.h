// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

// Event Dispatcher Define
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeHpSignature, float, CurHp, float, MaxHp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTKMK_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

	void InitializeStatus();
	void TakeDamage(float Damage);
	void UpdateUIHp();

	bool IsDeath();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurHp = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxHp = 1000.f;

	FOnChangeHpSignature OnChangeHp;
	FOnDeadSignature OnDead;
};
