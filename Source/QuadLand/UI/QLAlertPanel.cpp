// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLAlertPanel.h"
#include "Components/TextBlock.h"

void UQLAlertPanel::SetStatusTxt(const FString& CurrentStatus)
{
	Status->SetText(FText::FromString(CurrentStatus));
}


void UQLAlertPanel::SetNickNameTxt(const FString& InNickname)
{
	NickName->SetText(FText::FromString(InNickname));
	UE_LOG(LogTemp, Log, TEXT("%s"), *InNickname);
}
