// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;
class AAltar;

UENUM(BlueprintType)
enum class EPortalState : uint8
{
	EPS_Hidden,
	EPS_Opening,
	EPS_Open,
	EPS_Closing
};

UCLASS()
class PROJECTKMK_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOpeningEffectFinished(UParticleSystemComponent* PSystem);

public:	
	void HidePortal();
	void EnterOpening(AAltar* _Altar); //:ALTAR:  input param : Altar
	bool IsHidden();
	void EnterOpen();
	void MoveNextLevel(APawn* Target);
	void EnterClosing();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPortalVFX(EPortalState Type, bool bSpawn);
	void Multicast_PlayPortalVFX_Implementation(EPortalState Type, bool bSpawn);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangePortalView();
	void Multicast_ChangePortalView_Implementation();

	UFUNCTION()
	void OnRep_PortalState();

public:
	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Box;
	
	UPROPERTY(Transient)
	TObjectPtr<UParticleSystemComponent> CurrentPSC;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> VFX_OpeningTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> VFX_OpenTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> VFX_ClosingTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FString NextLevelName;

	UPROPERTY(ReplicatedUsing = OnRep_PortalState, VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	EPortalState PortalState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	TObjectPtr<AAltar> Altar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	TObjectPtr<ACameraActor> PortalCamera;
};
