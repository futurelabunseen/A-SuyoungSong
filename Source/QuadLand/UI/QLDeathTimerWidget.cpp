// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLDeathTimerWidget.h"
#include "Components/TextBlock.h"

void UQLDeathTimerWidget::UpdateTimer(int Time)
{
	if (Time <= 0)
	{
		TxtTimer->SetText(FText::FromString(TEXT("END")));
		return;
	}
	FString SecTxt = FString::Printf(TEXT("%d"), Time);
	TxtTimer->SetText(FText::FromString(SecTxt));
}
