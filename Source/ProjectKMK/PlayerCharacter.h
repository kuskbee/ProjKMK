// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/CombatReactInterface.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class UMotionWarpingComponent;
class AWeaponBase;
class ULegacyCameraShake;
class UNiagaraSystem;
class UStatusComponent;
class UTargetingSystemComponent;

UCLASS()
class PROJECTKMK_API APlayerCharacter : public ACharacter, public ICombatReactInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// CombatReactInteface Function
	virtual bool ApplyHit(const FHitResult& HitResult, AActor* HitterActor) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	
protected:

	// Bind Event
	void BindEventStatusComponent();

	UFUNCTION()
	void DoDeath(bool bIsDeadStatus);

	// Input Event Function
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnStopJump(const FInputActionValue& Value);
	void OnNormalAttack(const FInputActionValue& Value);
	void OnDashAttack(const FInputActionValue& Value);
	void OnLockOn(const FInputActionValue& Value);
	void OnDodge(const FInputActionValue& Value);
	void OnHold(const FInputActionValue& Value);
		
	bool IsMovable();
	void SetLocomotionState();

	// Combat
	bool IsCanAttack();
	void ActiveAttack(bool bIsDash);
	uint32 IncreaseAttackIndex();
	FString GetSectionNameFromHitDirection(FVector HitterLocation);
	float GetDegreeFromLocation(FVector Location);

	// Dash
	void GetAttackTargetActor();
	float GetTargetScoreByDistance(FVector Location);
	void SetWarpTarget();
	void ResetWarpTarget();

	// Knockback
	void SetKnockbackDirection(FVector KnockbackDirection);

	// Weapon
	void SpawnWeapon();

	// Effect
	void SpawnHitReactEffect(FVector Location);

	// Montage
	bool IsCanPlayMontage();
	void PlayAttackMontage(bool bIsDash);
	void UnbindEventAttackMontageEnd();
	void PlayHitReactMontage(FString SectionName);
	void UnbindEventHitReactMontageEnd();
	void PlayDeathMontage(FName SectionName);
	void PlayDodgeMontage();
	void UnbindEventDodgeMontageEnd();

	UFUNCTION()
	void EventAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void EventHitReactMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void EventDodgeMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	// TargetSystem
	void InitializeTargetSystem();
	void ChangeTargetMode(ETargetingMode CurrentMode);
	UFUNCTION()
	void SetNormalModeCamera();
	UFUNCTION()
	void SetTargetModeCamera();

public:
	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UStatusComponent> StatusComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UTargetingSystemComponent> TargetingSystem;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_NormalAttack;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_DashAttack;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Dodge;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hold;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(/*ReplicatedUsing = OnRep_EchoState, */EditAnywhere, Category = "State", BlueprintReadOnly)
	EPlayerState EchoState = EPlayerState::EPS_Locomotion;

	UPROPERTY(/*ReplicatedUsing = OnRep_IsHold, */VisibleAnywhere, Category = "State", BlueprintReadOnly)
	bool IsHold;

	UPROPERTY(/*ReplicatedUsing = OnRep_AttackIndex,*/ EditDefaultsOnly, Category = "Combat")
	int32 AttackIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 AttackSkillCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> NormalAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> DashAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(VisibleAnywhere, Category = "Montage", BlueprintReadOnly)
	int DeathIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AWeaponBase> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, Category = "Combat", BlueprintReadOnly)
	TObjectPtr<AWeaponBase> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> AttackCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> HitReactEffect;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Knockback")
	float KnockbackChance = 0.7f;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Combat | Dash")
	TObjectPtr<AActor> AttackTarget;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	TArray<FHitResult> AttackOutHits;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	TArray<TObjectPtr<AActor>> TargetActors;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	TArray<float> TargetScores;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	float AttackAngle = 60.f;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	TObjectPtr<AActor> PrevAttackTarget;
	UPROPERTY(VisibleAnywhere, Category = "Combat | Dash")
	float AttackRange = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "TargetSystem")
	ETargetingMode CurrentTargetingMode;

	UPROPERTY(EditDefaultsOnly, Category = "HoldTail")
	TSubclassOf<AActor> HoldTailClass;

	UPROPERTY(EditDefaultsOnly, Category = "HoldTail")
	TSubclassOf<AActor> TailClass;

	UPROPERTY(VisibleAnywhere, Category = "HoldTail")
	TObjectPtr<AActor> HoldTail;

public:
	__forceinline AWeaponBase* GetEquippedWeapon() { return EquippedWeapon; }

};
