// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLLobbyPlayerController.h"
#include "Game/QLLobbyGameMode.h"
AQLLobbyPlayerController::AQLLobbyPlayerController()
{
}

void AQLLobbyPlayerController::ServerRPCReady_Implementation(bool InReady)
{
	bIsReady = InReady;

	AQLLobbyGameMode *GameMode =Cast<AQLLobbyGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->ReadyPlayer();
	}

	//GetPawn�� �����ͼ� ���̷�Ż ����
}
