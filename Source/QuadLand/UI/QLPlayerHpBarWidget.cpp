// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLPlayerHpBarWidget.h"
#include "Components/ProgressBar.h"
UQLPlayerHpBarWidget::UQLPlayerHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHP = -1;
}

void UQLPlayerHpBarWidget::UpdateHPPercentage(float InHP, float InMaxHP)
{
	MaxHP = InMaxHP;
	CurrentHP = InHP;

	if (HP)
	{
		HP->SetPercent(CurrentHP / MaxHP);
	}
}

void UQLPlayerHpBarWidget::UpdateStaminaPercentage(float InStamina, float InMaxStamina)
{
	CurrentStamina = InStamina;
	MaxStamina = InMaxStamina;

	if (Stamina)
	{
		Stamina->SetPercent(CurrentStamina / MaxStamina);
	}
}

void UQLPlayerHpBarWidget::ShowCoolTimeStamina()
{
	if (Stamina)
	{
		Stamina->SetFillColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
	}
}

void UQLPlayerHpBarWidget::ResetStamina()
{
	if (Stamina)
	{
		Stamina->SetFillColorAndOpacity(OriginalColor);
	}
}

void UQLPlayerHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HP = Cast<UProgressBar>(GetWidgetFromName(TEXT("HP")));
	Stamina = Cast<UProgressBar>(GetWidgetFromName(TEXT("Stamina")));

	OriginalColor = Stamina->GetFillColorAndOpacity();
}
