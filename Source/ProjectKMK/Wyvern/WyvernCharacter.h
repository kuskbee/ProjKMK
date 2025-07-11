// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterInterface.h"
#include "../Interfaces/CombatReactInterface.h"
#include "ST_MyMonsterSkill.h"
#include "Define.h"
#include "WyvernCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMotionWarpingComponent;
class UMonStateComponent;
class UAnimMontage;
class UBehaviorTree;
class UNiagaraSystem;
class UParticleSystem;
class AMySurface;

UCLASS()
class PROJECTKMK_API AWyvernCharacter : public ACharacter, public IMonsterInterface, public ICombatReactInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWyvernCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// WyvernInterface Implement
	virtual float Attack() override;
	virtual bool AddTargetActor(AActor* InTarget) override;
	virtual bool RemoveTargetActor(AActor* InTarget) override;
	virtual AActor* ChangeTargetActor() override;
	virtual void CheckTargetActors() override;

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

	UFUNCTION(NetMulticast, Reliable)
	void S2A_OnAttack(UAnimMontage* InAttackMontage, float InPlayerRate);
	void S2A_OnAttack_Implementation(UAnimMontage* InAttackMontage, float InPlayerRate);

	UFUNCTION()
	void DoAttack(bool IsRightHand, bool IsMouth);

	UFUNCTION()
	void EventProcessTakePointDamage(AActor* DamagedActor, float In_Damage, class AController* InstigatedBy,
		FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
		const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_ApplyHit(const FHitResult& HitResult, AActor* HitterActor, float WarpingPercent, float RandomKnockbackPower);
	void S2A_ApplyHit_Implementation(const FHitResult& HitResult, AActor* HitterActor, float WarpingPercent, float RandomKnockbackPower);

	UFUNCTION()
	bool IsWeakAttack(FName BoneName);

	UFUNCTION()
	void CutTail(bool IsNotCut);

	UFUNCTION()
	void DoDeath();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_DoDeath();
	void S2A_DoDeath_Implementation();

	UFUNCTION()
	void DeadCollision();

	// Targeting System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AActor>> TargetActors;

	UFUNCTION(NetMulticast, Reliable)
	void ShowMonsterHealthBar(APlayerCharacter* InPlayer);
	void ShowMonsterHealthBar_Implementation(APlayerCharacter* InPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void HideMonsterHealthBar(APlayerCharacter* InPlayer);
	void HideMonsterHealthBar_Implementation(APlayerCharacter* InPlayer);
	//

	UFUNCTION()
	void EventUpdateMonPhase(EPhase In_Phase);

	UFUNCTION()
	bool IsMonsterMovable();

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float CurHP;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float MaxHP;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float Damage = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HowlingMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> RevivalMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> KnockBackMontage;

	UPROPERTY(EditAnywhere, Category = "Animations", BlueprintReadWrite)
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TSubclassOf<AActor> Tail;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	TSubclassOf<AMySurface> TailSurfaceClass;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<AMySurface> TailSurface;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> WeakAttackEffect;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	TObjectPtr<UParticleSystem> NotWeakAttackEffect;

	UPROPERTY(ReplicatedUsing=OnRep_Phase, EditAnywhere, Category = "Data", BlueprintReadWrite)
	EPhase Phase;

	UFUNCTION()
	void OnRep_Phase();

	UPROPERTY(ReplicatedUsing=OnRep_MonAIState, VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	EAIState MonAIState;

	UFUNCTION()
	void OnRep_MonAIState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
