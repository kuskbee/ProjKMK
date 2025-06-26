// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WyvernInterface.h"
#include "MyCombatReactInterface.h"
#include "Define.h"
#include "MySurface.h"
#include "WyvernCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDispatcherAttackEnd);

class USpringArmComponent;
class UCameraComponent;
class UMotionWarpingComponent;
class UMonStateComponent;
class UAnimMontage;
class UNiagaraSystem;
class UBehaviorTree;

UCLASS()
class PROJECTKMK_API AWyvernCharacter : public ACharacter, public IWyvernInterface, public IMyCombatReactInterface
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

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UMonStateComponent> MonStateComponent;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	TObjectPtr<UDataTable> FirstPhaseTable;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	TObjectPtr<UDataTable> SecondPhaseTable;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	TObjectPtr<UDataTable> ThirdPhaseTable;

	UPROPERTY(EditAnywhere, Category = "AI", BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> WyvernBehaviorTree;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual bool Attack() override;

	UFUNCTION(BlueprintCallable)
	virtual bool SetAIState(EAIState AIState) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ApplyHit(FHitResult HitResult, AActor* HitterActor) override;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcherAttackEnd EventAttackEnd;

	void SetMonState(FName RowName);
	void CutTail(bool IsNotCut);
	void EventMontageEnd(UAnimMontage* Montage, bool bINterrupted);

	void BattleTickOnFirstPhase();
	void BattleTickOnSecondPhase();
	void BattleTickOnThirdPhase();

	void DoAttack(bool IsRightHand, bool IsMouth);
	bool IsWeakAttack(FName BoneName);

	EPhase Phase;
	bool IsPlayMontage;
	EAIState MonAIState;
	float CurHP;
	float MaxHP;
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HowlingMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> RevivalMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> KnockBackMontage;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TSubclassOf<AMySurface> TailSurface;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> WeakAttackEffect;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> NotWeakAttackEffect;
};
