// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLBloodWidget.h"
#include "Components/Image.h"
#include "Misc/OutputDeviceNull.h"

UQLBloodWidget::UQLBloodWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UQLBloodWidget::NativeConstruct()
{
	FLinearColor Color(0.1f, 0.1f, 0.1f, 0.8f);
	BloodImage->SetColorAndOpacity(Color);
}

void UQLBloodWidget::BlinkWidget()
{
	FOutputDeviceNull Ar;
	CallFunctionByNameWithArguments(TEXT("K2_BlinkWidget"), Ar, nullptr, true);
}

void UQLBloodWidget::CancelWidget()
{
	FLinearColor Color(0.1f, 0.1f, 0.1f, 0.8f);
	BloodImage->SetColorAndOpacity(Color);
}
