// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLDefeat.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameMode.h"

void UQLDefeat::SetupDefeat()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();

	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{	FInputModeUIOnly UIOnlyInputMode;
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(UIOnlyInputMode);
			PlayerController->SetShowMouseCursor(true);
		}
	}


	if (ReturnLobbyButton)
	{
		ReturnLobbyButton->OnClicked.AddDynamic(this, &UQLDefeat::ReturnButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		if (MultiplayerSessionSubsystem)
		{
			MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UQLDefeat::OnDestorySession);
		}

	}
}

void UQLDefeat::OnDestorySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnLobbyButton->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();

	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDestorySession()"));
		AGameModeBase *GameMode = World->GetAuthGameMode<AGameModeBase>();

		if (GameMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnDestorySession() Server"));
			//서버
			
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			//클라이언트는 그냥 나가면된다
			UE_LOG(LogTemp, Warning, TEXT("OnDestorySession() Client"));
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;

			PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			
		}
	}
	
}

void UQLDefeat::ReturnButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerLeftGame()"));
	ReturnLobbyButton->SetIsEnabled(false);

	if (MultiplayerSessionSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessionsSubsystem valid"));
		MultiplayerSessionSubsystem->DestroySession();

	}
}
