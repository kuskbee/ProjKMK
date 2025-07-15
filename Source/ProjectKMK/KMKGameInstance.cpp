// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKGameInstance.h"
//#include "LoginClientSubsystem.h"
//#include "Kismet/GameplayStatics.h"
//#include "TimerManager.h"



//void UKMKGameInstance::Init()
//{
//	BindLoginClientEvent();
//}
//
//void UKMKGameInstance::BindLoginClientEvent()
//{
//	ULoginClientSubsystem* LoginSS = GetSubsystem<ULoginClientSubsystem>();
//
//	if (!IsValid(LoginSS))
//	{
//		GetWorld()->GetTimerManager().SetTimer(
//			RetryBindHandle,
//			this,
//			&UKMKGameInstance::BindLoginClientEvent,
//			0.01f,
//			false /*bLoop*/
//		);
//
//		return;
//	}
//
//	LoginSS->OnPlayerListResponseDelegate.AddDynamic(this, &UKMKGameInstance::OnResponsePlayerList);
//	LoginSS->OnPlayerInOutLobbyDelegate.AddDynamic(this, &UKMKGameInstance::OnPlayerInOutLobby);
//	LoginSS->OnPlayerChangeStateDelegate.AddDynamic(this, &UKMKGameInstance::OnPlayerChangeState);
//}
//
//void UKMKGameInstance::OnResponsePlayerList(const TArray<FPlayerInfo>& PlayerInfos)
//{
//	UE_LOG(LogTemp, Warning, TEXT("[UKMKGameInstance] OnResponsePlayerList, Player Num : %d "),
//		PlayerInfos.Num());
//
//	for (auto& Player : PlayerInfos)
//	{
//		PlayerList.Add(Player.UserId, Player);
//	}
//
//	EventDispatcher_PlayerList.broadcast();
//	EventDispatcher_ChangePlayerCnt.broadcast(PlayerList.Num());
//}
//
//void UKMKGameInstance::OnPlayerInOutLobby(FPlayerInfo PlayerInfo, bool bIsJoin)
//{
//	UE_LOG(LogTemp, Log, TEXT("[UKMKGameInstance] OnPlayerInOutLobby %s %s Lobby")
//		, *PlayerInfo.Nickname, bIsJoin ? TEXT("Enter") : TEXT("Leave"));
//
//	if (bIsJoin)
//	{
//		PlayerList.Add(PlayerInfo.UserId, PlayerInfo);
//
//		EventDispatcher_AddPlayerOne.broadcast(PlayerInfo.Nickname);
//	}
//	else
//	{
//		PlayerList.Remove(PlayerInfo.UserId);
//		EventDispatcher_RemovePlayerOne.broadcast(PlayerInfo.Nickname);
//	}
//
//	EventDispatcher_ChangePlayerCnt.broadcast(PlayerList.Num());
//}
//
//void UKMKGameInstance::OnPlayerChangeState(FString UserId, EEntryState CurrentState)
//{
//	UE_LOG(LogTemp, Log, TEXT("[UKMKGameInstance] OnPlayerChangeState %s State ¡æ %d"),
//		*UserId, static_cast<uint8>(CurrentState));
//
//	FPlayerInfo* Player = PlayerList.Find(UserId);
//	if (!Player)
//	{
//		UE_LOG(LogTemp, Log, TEXT("[UKMKGameInstance] OnPlayerChangeState Not Found User [%s]"), *UserId);
//		return;
//	}
//
//	PlayerList[UserId].CurrentState = CurrentState;
//
//	bool bIsReady = CurrentState == EEntryState::EPS_Ready;
//	EventDispatcher_PlayerReady.broadcast(PlayerList[UserId].Nickname, bIsReady);
//
//	if (this->Nickname == PlayerList[UserId].Nickname)
//	{
//		EventDispatcher_ChangeMyReadState(bIsReady);
//	}
//}
