// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLReturnToLobby.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Player/QLPlayerController.h"
#include "GameFramework/GameMode.h"
#include "Components/TextBlock.h"
#include "QuadLand.h"

void UQLReturnToLobby::SetupUI()
{
	bIsFocusable = true;

	if (ReturnLobbyButton && !ReturnLobbyButton->OnClicked.IsBound())
	{
		ReturnLobbyButton->OnClicked.AddDynamic(this, &UQLReturnToLobby::ReturnButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby 2"));
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby 3"));
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		if (MultiplayerSessionSubsystem)
		{
			MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UQLReturnToLobby::OnDestorySession);
			MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UQLReturnToLobby::OnDestorySession);
		}

	}
}

void UQLReturnToLobby::OnDestorySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby cancel"));
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

			//모든 클라이언트한테 나가라고 한다.
			UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby server 4"));
			GameMode->ReturnToMainMenuHost();
		}
		else
		{

			UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby clinet 4"));
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

void UQLReturnToLobby::SettingTxtPhase(const FString& Nickname)
{
	if (TxtPhrase)
	{
		FString Phrase = TxtPhrase->GetText().ToString();
		FString ChangePhrase=Phrase.Replace(*Delimiter, *Nickname);

		TxtPhrase->SetText(FText::FromString(ChangePhrase));
	}
}

void UQLReturnToLobby::NativeConstruct()
{
	if (ReturnLobbyButton == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("UQLReturnToLobby 1"));
		ReturnLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("ReturnLobbyButton")));
	}
}
