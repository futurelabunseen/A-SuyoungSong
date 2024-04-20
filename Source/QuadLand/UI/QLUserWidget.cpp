// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLUserWidget.h"
#include "UI/QLPlayerHpBarWidget.h"
#include "UI/QLPlayerHUDWidget.h"

UQLUserWidget::UQLUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UQLUserWidget::ChangedHPPercentage(float InHP,float InMaxHP)
{
	if (HpBar)
	{
		HpBar->UpdateHPPercentage(InHP, InMaxHP);
	}
}

void UQLUserWidget::ChangedAmmoCnt(float InAmmo)
{
	if (CharacterStat)
	{
		CharacterStat->UpdateAmmo(InAmmo);
	}
}

void UQLUserWidget::UpdateRemainingAmmo(float InAmmo)
{
	if (CharacterStat)
	{
		CharacterStat->UpdateRemainingAmmo(InAmmo);
	}
}

void UQLUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//√ ±‚»≠ 
	HpBar = Cast<UQLPlayerHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);
	CharacterStat = Cast<UQLPlayerHUDWidget>(GetWidgetFromName(TEXT("WidgetAmmoCnt")));//WidgetAmmoCnt
	ensure(CharacterStat);
}
