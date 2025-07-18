// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTKMK_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWeaponCollisionEnable(bool IsEnable);
protected:
	void ClearIgnoreActors();
	bool HasSameTag(AActor* TargetActor);
	bool DoBoxTrace(AActor* TargetActor, FHitResult& OutHit);
	bool HasSameMonsterTag(AActor* TargetActor);

	/*UFUNCTION()
	void OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);*/

public: 
	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<USceneComponent> BoxTraceEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AActor>> IgnoreActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 500.f;
};
