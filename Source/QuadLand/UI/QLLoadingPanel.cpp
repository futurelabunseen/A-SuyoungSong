// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLLoadingPanel.h"
#include "Components/TextBlock.h"

void UQLLoadingPanel::SetTxtRemainingTime(uint32 Time)
{
	if (TxtRemainingTime->IsVisible() == false)
	{
		TxtRemainingTime->SetVisibility(ESlateVisibility::Visible);
	}
	FString RemainingTime=FString::Printf(TEXT("%d"), Time);

	TxtRemainingTime->SetText(FText::FromString(RemainingTime));
}

void UQLLoadingPanel::NativeConstruct()
{
	Super::NativeConstruct();
	TxtRemainingTime->SetVisibility(ESlateVisibility::Hidden);
}
