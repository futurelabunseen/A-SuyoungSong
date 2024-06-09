// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLSwitchWeaponWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "QuadLand.h"

UQLSwitchWeaponWidget::UQLSwitchWeaponWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), 
DeactivePunchScale(0.6f, 0.9), ActivePunchScale(0.9f, 0.8f),
DeactiveGunScale(1.0f, 1.0f), ActiveGunScale(2.0f, 2.0f),
DeactiveBombScale(0.6f, 0.8f), ActiveBombScale(0.6f, 1.4f),
DiactiveColor(1.0f, 1.0f, 1.f, 0.3f), ActiveColor(1.0f, 1.0f, 1.f, 1.0f)
{
}

void UQLSwitchWeaponWidget::NativeConstruct()
{
	//ÃÊ±âÈ­
	if (IMGBomb && IMGGun && IMGPunch)
	{
		IMGPunch->SetColorAndOpacity(ActiveColor);
		IMGPunch->SetRenderScale(ActivePunchScale);

		IMGGun->SetColorAndOpacity(DiactiveColor);
		IMGGun->SetRenderScale(DeactiveGunScale);

		IMGBomb->SetColorAndOpacity(DiactiveColor);
		IMGBomb->SetRenderScale(DeactiveBombScale);
	}

	IMGBomb->SetVisibility(ESlateVisibility::Hidden);
	IMGGun->SetVisibility(ESlateVisibility::Hidden);
}

void UQLSwitchWeaponWidget::SwitchWeaponStyle(ECharacterAttackType AttackType)
{
	
	if (AttackType == ECharacterAttackType::BombAttack)
	{

		IMGBomb->SetColorAndOpacity(ActiveColor);
		IMGBomb->SetRenderScale(ActiveBombScale);

		IMGGun->SetColorAndOpacity(DiactiveColor);
		IMGGun->SetRenderScale(DeactiveGunScale);

		IMGPunch->SetColorAndOpacity(DiactiveColor);
		IMGPunch->SetRenderScale(DeactivePunchScale);
		UE_LOG(QLNetLog, Warning, TEXT("UQLSwitchWeaponWidget : Current Attack Type %d"), AttackType);

	}
	else if(AttackType == ECharacterAttackType::HookAttack)
	{

		IMGPunch->SetColorAndOpacity(ActiveColor);
		IMGPunch->SetRenderScale(ActivePunchScale);

		IMGGun->SetColorAndOpacity(DiactiveColor);
		IMGGun->SetRenderScale(DeactiveGunScale);

		IMGBomb->SetColorAndOpacity(DiactiveColor);
		IMGBomb->SetRenderScale(DeactiveBombScale);
		UE_LOG(QLNetLog, Warning, TEXT("UQLSwitchWeaponWidget : Current Attack Type %d"), AttackType);

	}
	else
	{
		IMGGun->SetColorAndOpacity(ActiveColor);
		IMGGun->SetRenderScale(ActiveGunScale);

		IMGBomb->SetColorAndOpacity(DiactiveColor);
		IMGBomb->SetRenderScale(DeactiveBombScale);

		IMGPunch->SetColorAndOpacity(DiactiveColor);
		IMGPunch->SetRenderScale(DeactivePunchScale);
		UE_LOG(QLNetLog, Warning, TEXT("UQLSwitchWeaponWidget : Change Gun"));

	}
}

void UQLSwitchWeaponWidget::UpdateEquipWeaponUI()
{
	if (IMGGun->IsVisible())
	{
		IMGGun->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		IMGGun->SetVisibility(ESlateVisibility::Visible);
	}
}

void UQLSwitchWeaponWidget::UpdateEquipBombUI()
{
	if (IMGBomb->IsVisible())
	{
		IMGBomb->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		IMGBomb->SetVisibility(ESlateVisibility::Visible);
	}
}

//punch 0.6	0.9 -> 0.9 1.0
//default 1 1 -> 2 2 = gun
//default 0.6 0.8 -> 0.6 1.4 = bomb