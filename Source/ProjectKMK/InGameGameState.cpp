// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "UI/MyHUD.h"

AInGameGameState::AInGameGameState()
{
	bReplicates = true;
	CurrentGameState = EGameState::EGS_None;
}

void AInGameGameState::BeginPlay()
{
	Super::BeginPlay();

	//���� ���� �� �ʱ� ���� ����(���� Ready ���� - �ּ�ó��)
	/*if (HasAuthority())
	{
		SetCurrentGameState(EGameState::EGS_Ready);
	}*/

}

void AInGameGameState::OnRep_CurrentGameState()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
		{
			LocalHud->EventChangeGameState(CurrentGameState);
		}
	}

	OnGameStateChanged.Broadcast(CurrentGameState);
}

void AInGameGameState::SetCurrentGameState(EGameState NewState)
{
	//���������� ���� ����� ���� ȣ��
	if (HasAuthority() && CurrentGameState != NewState)
	{
		//�������� ���� ����
		CurrentGameState = NewState;

		//Listen Server �뵵
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (AMyHUD* LocalHud = Cast<AMyHUD>(PC->GetHUD()))
			{
				LocalHud->EventChangeGameState(CurrentGameState);
			}
		}//Dedicated Server ��� �� �ּ�(����)ó���� ��

		OnGameStateChanged.Broadcast(CurrentGameState);
	}
}

void AInGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInGameGameState, CurrentGameState);
}
