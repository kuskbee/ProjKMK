// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

UCLASS()
class PROJECTKMK_API APlayerCharacter : public ACharacter, public ICombatReactInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

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

protected:

	// Input Event Function
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnStopJump(const FInputActionValue& Value);
	void OnNormalAttack(const FInputActionValue& Value);
		
	void SetLocomotionState();

	// Combat
	bool IsCanAttack();
	void ActiveAttack(bool bIsDash);
	uint32 IncreaseAttackIndex();
	FString GetSectionNameFromHitDirection(FVector HitterLocation);
	float GetDegreeFromLocation(FVector Location);

	// Dash
	void RemoveSyncPoint();

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


	UFUNCTION()
	void EventAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void EventHitReactMontageEnd(UAnimMontage* Montage, bool bInterrupted);


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
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, Category = "State", BlueprintReadOnly)
	EPlayerState EchoState = EPlayerState::EPS_Locomotion;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	bool IsHold;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 AttackIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 AttackSkillCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> NormalAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> DashAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere, Category = "Montage", BlueprintReadOnly)
	int DeathIndex;

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

public:
	__forceinline AWeaponBase* GetEquippedWeapon() { return EquippedWeapon; }

};
