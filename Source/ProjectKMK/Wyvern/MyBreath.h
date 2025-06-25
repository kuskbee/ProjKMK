// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBreath.generated.h"

class UParticleSystemComponent;
class USphereComponent;

UCLASS()
class PROJECTKMK_API AMyBreath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyBreath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UParticleSystemComponent> Particle;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<USphereComponent> End;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<USphereComponent> Start;

	UPROPERTY(VisibleAnywhere, Category = "Data", BlueprintReadOnly)
	float Damage;

	UFUNCTION(BlueprintCallable)
	void BreathDamage();

};
