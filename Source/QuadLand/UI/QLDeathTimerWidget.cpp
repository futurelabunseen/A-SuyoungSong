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

//void UQLDeathTimerWidget::SetTxt()
//{
//	static int Sec = 10;
//	if (Sec <= 0)
//	{
//		TxtTimer->SetText(FText::FromString(TEXT("END")));
//		return;
//	}
//	FString SecTxt = FString::Printf(TEXT("%d"), Sec--);
//	TxtTimer->SetText(FText::FromString(SecTxt));
//
//}
//
//void UQLDeathTimerWidget::ClearTimer()
//{
//	GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
//	DeathTimerHandle.Invalidate();
//}
