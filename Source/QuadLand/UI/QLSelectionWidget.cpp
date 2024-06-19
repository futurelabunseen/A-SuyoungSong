// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/QLLobbyPlayerState.h"
#include "Game/QLGameInstance.h"
#include "Player/QLLobbyPlayerController.h"
void UQLSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (TxtWait)
	{
		TxtWait->SetVisibility(ESlateVisibility::Hidden);
	}
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

void UQLSelectionWidget::ShowWaitTxt()
{
	TxtWait->SetVisibility(ESlateVisibility::Visible);
}

void UQLSelectionWidget::ShowLimitTxt()
{

	TimeLimitSec -= 1;
	FString RemainingTimeTxt = FString::Printf(TEXT("%d"), TimeLimitSec);
	TxtWait->SetText(FText::FromString(RemainingTimeTxt));
	
	if (TimeLimitSec <= 1)
	{
		AQLLobbyPlayerController* PC = GetOwningPlayer<AQLLobbyPlayerController>();
		if (PC)
		{
			GetWorld()->GetTimerManager().ClearTimer(LimitTimer);
			LimitTimer.Invalidate();
			PC->ServerRPCTimeout();
		}
	}
}

void UQLSelectionWidget::StartLimitTimer()
{
	CheckTimeLimit = true;
	ShowLimitTxt();
	TxtWait->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(LimitTimer, this, &UQLSelectionWidget::ShowLimitTxt, 1.f, true);
}

void UQLSelectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//if (CheckTimeLimit)
	//{
	//	FString RemainingTimeTxt = FString::Printf(TEXT("%d"), TimeLimitSec);
	//	TxtWait->SetText(FText::FromString(RemainingTimeTxt));

	//	UE_LOG(LogTemp, Log, TEXT(" %d "), TimeLimitSec);
	//	if (TimeLimitSec >= 0)
	//	{
	//		AQLLobbyPlayerController *PC = GetOwningPlayer<AQLLobbyPlayerController>();

	//		if (PC)
	//		{
	//			PC->ServerRPCTimeout();
	//		}
	//	}
	//}
}


