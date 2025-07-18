// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "../KMKGameInstance.h"
#include "../UI/MyHUD.h"
#include "TimerManager.h"

void AInGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInGamePlayerState, UserNickname);
}

void AInGamePlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (const UKMKGameInstance* KMKGI = GetGameInstance<UKMKGameInstance>())
	{
		ServerSetNickname(KMKGI->Nickname);
	}

	if (!IsNetMode(NM_DedicatedServer))
	{
		BindEventToHUD();
	}
}

void AInGamePlayerState::BindEventToHUD()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
		{
			LocalHud->BindPlayerStateEvent(this);
			LocalHud->UpdatePlayerListUI();

			return;
		}
	}
	
	FTimerHandle TempBindTimer;
	GetWorld()->GetTimerManager().SetTimer(TempBindTimer, this, &AInGamePlayerState::BindEventToHUD, 0.1f);
}

void AInGamePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void AInGamePlayerState::OnRep_Nickname()
{
	OnSetUserNicknameDelegate.Broadcast(UserNickname);
}

void AInGamePlayerState::ServerSetNickname_Implementation(const FString& Nickname)
{
	UE_LOG(LogTemp, Warning, TEXT("[AKMKPlayerController] ServerSetNickname [%s]"), *Nickname);

	UserNickname = Nickname;

	ForceNetUpdate();
}
