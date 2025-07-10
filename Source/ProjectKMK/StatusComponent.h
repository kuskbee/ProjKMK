// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "StatusComponent.generated.h"


// Event Dispatcher Define
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadSignature, bool, bIsDeadStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeHpSignature, float, CurHp, float, MaxHp);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTKMK_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeStatus();
	void TakeDamage(float Damage);
	void UpdateUIHp();
	void SetHp(float NewHp);
	void SetIsDead(bool bNewDeadStatus);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_CurHp, EditAnywhere, BlueprintReadOnly)
	float CurHp = 0.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float MaxHp = 1000.0f;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, EditAnywhere, BlueprintReadOnly)
	bool bIsDead = false;
	UPROPERTY()
	bool bHasBeenDead = false;

	//RepNotify Functions
	UFUNCTION()
	void OnRep_CurHp();
	UFUNCTION()
	void OnRep_IsDead();

public:

	//Getter Functions
	float GetCurHp() const {	return CurHp; }
	float GetMaxHp() const { return MaxHp; }
	bool CalculateIsDead() const { return (CurHp <= 0.f); }
	bool GetIsDead() const { return bIsDead; }

	UPROPERTY(BlueprintAssignable)
	FOnChangeHpSignature OnChangeHp;
	UPROPERTY(BlueprintAssignable)
	FOnDeadSignature OnDead;
};
