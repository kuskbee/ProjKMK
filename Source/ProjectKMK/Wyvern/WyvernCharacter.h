// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WyvernInterface.h"
#include "../Interfaces/CombatReactInterface.h"
#include "ST_MyMonsterSkill.h"
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
class PROJECTKMK_API AWyvernCharacter : public ACharacter, public IWyvernInterface, public ICombatReactInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWyvernCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// WyvernInterface Implement
	virtual void Attack() override;

	// CombatReactInterface Implement
	virtual bool ApplyHit(const FHitResult& HitResult, AActor* HitterActor) override;

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

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcherAttackEnd EventAttackEnd;

	//UFUNCTION(Server, Reliable, WithValidation)
	//void C2S_Attack();
	//void C2S_Attack_Implementation();
	//bool C2S_Attack_Validate();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_OnAttack(UAnimMontage* InAttackMontage, float InPlayerRate);
	void S2A_OnAttack_Implementation(UAnimMontage* InAttackMontage, float InPlayerRate);

	UFUNCTION()
	void DoAttack(bool IsRightHand, bool IsMouth);

	UFUNCTION()
	void SetMonState(EPhase InPhase);

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

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
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
