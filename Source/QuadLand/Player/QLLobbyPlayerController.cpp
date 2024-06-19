// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLLobbyPlayerController.h"
#include "Game/QLLobbyGameMode.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "UI/QLSelectionWidget.h"
AQLLobbyPlayerController::AQLLobbyPlayerController()
{
}

void AQLLobbyPlayerController::ServerRPCTimeout_Implementation()
{
	AQLLobbyGameMode *GM = Cast<AQLLobbyGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->GameStart();
	}
}

void AQLLobbyPlayerController::SettingMesh(int Idx)
{
	ACharacter* PlayerPawn = GetPawn<ACharacter>();

	if (PlayerPawn)
	{
		PlayerPawn->GetMesh()->SetSkeletalMesh(Meshs[Idx]);
	}
}

void AQLLobbyPlayerController::ClientRPCActivateTimeLimit_Implementation()
{
	UQLSelectionWidget *Selector = Cast<UQLSelectionWidget>(HUD);

	if (Selector)
	{
		Selector->StartLimitTimer();
	}
}

void AQLLobbyPlayerController::ClientRPCShowWaitUI_Implementation()
{
	UQLSelectionWidget* Selector = Cast<UQLSelectionWidget>(HUD);

	if (Selector)
	{
		Selector->ShowWaitTxt();
	}
}

void AQLLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		CreateHUD();
	}
}

void AQLLobbyPlayerController::CreateHUD()
{

	if (!IsLocalPlayerController())
	{
		return;
	}

	FInputModeUIOnly UIOnlyInputMode;
	SetInputMode(UIOnlyInputMode);
	SetShowMouseCursor(true);

	HUD = CreateWidget<UUserWidget>(this, HUDClass);

	if (HUD)
	{
		HUD->AddToViewport();
	}

}

void AQLLobbyPlayerController::ServerRPCReady_Implementation(bool InReady)
{
	bIsReady = InReady;
	//GetPawn을 가져와서 스켈레탈 변경
	AQLLobbyGameMode* GM = Cast<AQLLobbyGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->ReadyPlayer();
	}
}
