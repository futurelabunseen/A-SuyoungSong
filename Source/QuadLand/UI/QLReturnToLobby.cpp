// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLReturnToLobby.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameMode.h"
#include "QuadLand.h"
void UQLReturnToLobby::SetupUI()
{
	bIsFocusable = true;

	if (ReturnLobbyButton)
	{
		ReturnLobbyButton->OnClicked.AddDynamic(this, &UQLReturnToLobby::ReturnButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		if (MultiplayerSessionSubsystem)
		{
			MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UQLReturnToLobby::OnDestorySession);
		}

	}
}

void UQLReturnToLobby::OnDestorySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnLobbyButton->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AGameMode* GameMode = World->GetAuthGameMode<AGameMode>();
		if (GameMode)
		{
			//서버
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			//클라이언트는 그냥 나가면된다
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;

			PlayerController->ClientReturnToMainMenuWithTextReason(FText());
		}
	}

}

void UQLReturnToLobby::ReturnButtonClicked()
{
	ReturnLobbyButton->SetIsEnabled(false);

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->DestroySession();
	}
}

void UQLReturnToLobby::NativeConstruct()
{
	if (ReturnLobbyButton == nullptr)
	{
		ReturnLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("ReturnLobbyButton")));
	}
}
