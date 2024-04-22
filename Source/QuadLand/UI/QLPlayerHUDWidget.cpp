// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLPlayerHUDWidget.h"
#include "Components/TextBlock.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Player/QLPlayerController.h"

UQLPlayerHUDWidget::UQLPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	CurrentAmmoCnt = 0;
	RemainingAmmoCnt = 0; 
}

void UQLPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentAmmoCntTxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtCurrentAmmoCnt")));
	RemainingAmmoCntTxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtRemainingAmmoCnt")));
}

void UQLPlayerHUDWidget::UpdateAmmo(float Ammo)
{ 
	CurrentAmmoCnt = Ammo;

	if (CurrentAmmoCntTxt)
	{
		FString CurrentAmmoTxt = FString::Printf(TEXT("%d"), CurrentAmmoCnt);
		CurrentAmmoCntTxt->SetText(FText::FromString(CurrentAmmoTxt));
	}
}

void UQLPlayerHUDWidget::UpdateRemainingAmmo(float AmmoCnt)
{
	RemainingAmmoCnt = AmmoCnt;

	if (RemainingAmmoCntTxt)
	{
		FString RemainingAmmoTxt = FString::Printf(TEXT("%d"), RemainingAmmoCnt);
		RemainingAmmoCntTxt->SetText(FText::FromString(RemainingAmmoTxt));
	}
}
