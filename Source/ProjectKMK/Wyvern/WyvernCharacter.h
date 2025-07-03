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
class UBehaviorTree;
class UNiagaraSystem;
class UParticleSystem;

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

	virtual void PossessedBy(AController* NewController) override;

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

	UFUNCTION(BlueprintCallable)
	virtual bool Attack() override;

	UFUNCTION(BlueprintCallable)
	virtual bool ApplyHit(FHitResult HitResult, AActor* HitterActor) override;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcherAttackEnd EventAttackEnd;

	UFUNCTION()
	void DoAttack(bool IsRightHand, bool IsMouth);

	UFUNCTION()
	void SetMonState(FName RowName);

	UFUNCTION()
	void CutTail(bool IsNotCut);

	UFUNCTION()
	void EventMontageEnd(UAnimMontage* Montage, bool bINterrupted);

	UFUNCTION()
	void EventUpdateMonPhase(EPhase In_Phase);

	UFUNCTION()
	void EventProcessTakePointDamage(AActor* DamagedActor, float In_Damage, class AController* InstigatedBy,
		FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, 
		const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	bool IsWeakAttack(FName BoneName);

	UFUNCTION()
	void InitAI(UObject* NewController);

	UFUNCTION()
	void UpdateWalkSpeed(float NewWalkSpeed);

	UFUNCTION()
	bool IsMonsterMovable();

	UFUNCTION()
	void DoDeath();

	UFUNCTION()
	void DeadCollision();

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	EPhase Phase;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	bool IsPlayMontage;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	EAIState MonAIState;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float CurHP;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float MaxHP;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HowlingMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> RevivalMontage;

	UPROPERTY(VisibleAnywhere, Category = "Animations", BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> KnockBackMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TObjectPtr<AMySurface> TailSurface;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> WeakAttackEffect;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UParticleSystem> NotWeakAttackEffect;
};
