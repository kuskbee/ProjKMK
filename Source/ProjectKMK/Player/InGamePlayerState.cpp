// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "../KMKGameInstance.h"

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
