// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLSelectionWidget.h"
#include "Components/Button.h"
#include "Player/QLLobbyPlayerState.h"
#include "Game/QLGameInstance.h"
#include "Player/QLLobbyPlayerController.h"
void UQLSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UQLSelectionWidget::ChangeGenderTypeToInt(FName InGender)
{
	TArray<FString> OutArray;

	FString GenderType = InGender.ToString();

	GenderType.ParseIntoArray(OutArray, TEXT("_"));

	if (OutArray.Num() > 1)
	{
		Gender = FCString::Atoi(OutArray[1].GetCharArray().GetData()) - 1;

		AQLLobbyPlayerState *PS = GetOwningPlayerState<AQLLobbyPlayerState>();
		if (PS)
		{
			PS->SetGenderType(Gender);
		}
	}

	UQLGameInstance* GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		GameInstance->SetGenderType(Gender);
	}
}

void UQLSelectionWidget::ChangeGemTypeToInt(FName GemColor)
{
	TArray<FString> OutArray;

	FString GemType = GemColor.ToString();

	GemType.ParseIntoArray(OutArray, TEXT("_"));

	if (OutArray.Num()>1)
	{
		Gem = FCString::Atoi(OutArray[1].GetCharArray().GetData()) -1 ;

		AQLLobbyPlayerState* PS = GetOwningPlayerState<AQLLobbyPlayerState>();
		if (PS)
		{
			PS->SetGemType(Gem);
		}
	}
	

	UQLGameInstance *GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		GameInstance->SetGemType(Gem);
	}
}

void UQLSelectionWidget::SetReady()
{
	AQLLobbyPlayerController*LobbyController = Cast<AQLLobbyPlayerController>(GetOwningPlayer());

	if (LobbyController)
	{
		LobbyController->ServerRPCReady(true);
	}
}

