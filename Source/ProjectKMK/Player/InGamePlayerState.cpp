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
	if (const UKMKGameInstance* KMKGI = GetGameInstance<UKMKGameInstance>())
	{
		ServerSetNickname(KMKGI->Nickname);
	}

	BindEventToHUD();
}

void AInGamePlayerState::BindEventToHUD()
{
	bool bBind = false;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
		{
			LocalHud->BindPlayerStateEvent(this);
			LocalHud->UpdatePlayerListUI();

			bBind = true;
		}
	}

	if (!bBind)
	{
		FTimerHandle TempBindTimer;
		GetWorld()->GetTimerManager().SetTimer(TempBindTimer, this, &AInGamePlayerState::BindEventToHUD, 0.1f);
	}
}

void AInGamePlayerState::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed : %s"), *GetName());
	Super::Destroyed();
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
//
//void AInGamePlayerState::Multicast_PlayerJoined_Implementation(AInGamePlayerState* NewPS)
//{
//	OnPlayerJoinedDelegate.Broadcast(NewPS);
//}
//
//void AInGamePlayerState::Multicast_PlayerLeft_Implementation(AInGamePlayerState* LeftPS)
//{
//	UE_LOG(LogTemp, Warning, TEXT("[AInGameGameState] Multicast_PlayerLeft %s"), *LeftPS->GetName());
//	OnPlayerLeftDelegate.Broadcast(LeftPS);
//}
//
