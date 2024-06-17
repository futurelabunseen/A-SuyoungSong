// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLLobbyPlayerController.h"
#include "Game/QLLobbyGameMode.h"
AQLLobbyPlayerController::AQLLobbyPlayerController()
{
}

void AQLLobbyPlayerController::ServerRPCReady_Implementation(bool InReady)
{
	bIsReady = InReady;
	//GetPawn을 가져와서 스켈레탈 변경
}
