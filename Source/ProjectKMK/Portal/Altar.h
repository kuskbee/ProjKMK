// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Altar.generated.h"

class UParticleSystem;
class UParticleSystemComponent;
class UBoxComponent;
class APortal;

UCLASS()
class PROJECTKMK_API AAltar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAltar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ShowOpenEffect();

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> AltarMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(Transient)
	TObjectPtr<UParticleSystemComponent> ParticleSystem;

	UPROPERTY(EditDefaultsOnly, Category="VFX")
	TObjectPtr<UParticleSystem> OpenEffectTemplate;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TObjectPtr<APortal> LevelMovePortal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsOverlappedTail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurOverlappedSeconds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float OverlappedThresholdSeconds = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsTriggerOpen;

};
