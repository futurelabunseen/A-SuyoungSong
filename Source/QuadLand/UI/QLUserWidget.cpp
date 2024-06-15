// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLUserWidget.h"
#include "UI/QLSwitchWeaponWidget.h"
#include "UI/QLPlayerHpBarWidget.h"
#include "UI/QLPlayerHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "QuadLand.h"
UQLUserWidget::UQLUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsSemiAutomatic = true;
}

void UQLUserWidget::ChangedHPPercentage(float InHP,float InMaxHP)
{
	if (HpBar)
	{
		HpBar->UpdateHPPercentage(InHP, InMaxHP);
	}
}

void UQLUserWidget::ChangedStaminaPercentage(float InStamina, float InMaxStamina)
{
	if (HpBar)
	{
		HpBar->UpdateStaminaPercentage(InStamina, InMaxStamina);
	}
}

void UQLUserWidget::SwitchWeaponStyle(ECharacterAttackType AttackType)
{
	SwitchWeapon->SwitchWeaponStyle(AttackType);
}

void UQLUserWidget::VisibleShootingMethodUI(bool InbIsSemiAutomatic,bool Hidden)
{

	if (HiddenTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(HiddenTimer);
		HiddenTimer.Invalidate();
	}

	if (InbIsSemiAutomatic!=bIsSemiAutomatic)
	{ 
		if (InbIsSemiAutomatic)
		{
			TxtShootingMethod->SetText(FText::FromString(TEXT("단발")));
		}
		else
		{
			TxtShootingMethod->SetText(FText::FromString(TEXT("연사")));
		}
		bIsSemiAutomatic = InbIsSemiAutomatic;
	}

	TxtShootingMethod->SetVisibility(ESlateVisibility::Visible);
	
	GetWorld()->GetTimerManager().SetTimer(HiddenTimer, this, &UQLUserWidget::HiddenShootingMethodUI, 3.0f, false);

}

void UQLUserWidget::HiddenShootingMethodUI()
{
	TxtShootingMethod->SetVisibility(ESlateVisibility::Hidden);
	HiddenTimer.Invalidate();
}

void UQLUserWidget::BlinkBag()
{
	if (IMGContainer)
	{
		PlayAnimation(Blink);
	}
}

void UQLUserWidget::ConcealLifeStone()
{
	if (IMGStone)
	{
		if (IMGStone->IsVisible())
		{
			IMGStone->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			IMGStone->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UQLUserWidget::UpdateEquipWeaponUI()
{
	if (SwitchWeapon)
	{
		SwitchWeapon->UpdateEquipWeaponUI();
	}
}

void UQLUserWidget::UpdateEquipBombUI()
{
	if (SwitchWeapon)
	{
		SwitchWeapon->UpdateEquipBombUI();
	}
}


void UQLUserWidget::ChangedAmmoCnt(float InAmmo)
{
	if (CharacterStat)
	{
		CharacterStat->UpdateAmmo(InAmmo);
	}
}

void UQLUserWidget::ChangedRemainingAmmo(float InAmmo)
{
	if (CharacterStat)
	{
		CharacterStat->UpdateRemainingAmmo(InAmmo);
	}
}

void UQLUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 
	HpBar = Cast<UQLPlayerHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);
	CharacterStat = Cast<UQLPlayerHUDWidget>(GetWidgetFromName(TEXT("WidgetAmmoCnt")));//WidgetAmmoCnt
	ensure(CharacterStat);
	SwitchWeapon = Cast<UQLSwitchWeaponWidget>(GetWidgetFromName(TEXT("WidgetSwitchWeapon")));
	ensure(SwitchWeapon);
	TxtShootingMethod = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtShootingMethod")));
	ensure(TxtShootingMethod);

	TxtShootingMethod->SetVisibility(ESlateVisibility::Hidden);
}

void UQLUserWidget::UpdateLivePlayer(int16 InLivePlayer)
{
	if (TxtPersonCnt)
	{
		FString TxtLivePlayer = FString::Printf(TEXT("%d"), InLivePlayer);
		TxtPersonCnt->SetText(FText::FromString(TxtLivePlayer));
	}
}

void UQLUserWidget::UpdateProgressTime(const FString &InTime)
{
	if (TxtProgressTime)
	{
		TxtProgressTime->SetText(FText::FromString(InTime));
	}
}

void UQLUserWidget::SettingNickname(const FString& InNickname)
{
	if (TxtNickname)
	{
		TxtNickname->SetText(FText::FromString(InNickname));
	}
}

void UQLUserWidget::SettingStoneImg(UTexture2D *StoneImg)
{
	if (IMGStone)
	{
		IMGStone->SetBrushFromTexture(StoneImg);
	}
}
