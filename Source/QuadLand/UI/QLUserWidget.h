// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/WeaponType.h"
#include "QLUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQLUserWidget(const FObjectInitializer& ObjectInitializer);
	
	void ChangedAmmoCnt(float InAmmo);
	void ChangedRemainingAmmo(float InAmmo);
	void ChangedHPPercentage(float InHP,float InMaxHP);
	void ChangedStaminaPercentage(float InStamina, float InMaxStamina);
	void SwitchWeaponStyle(ECharacterAttackType AttackType);

	void VisibleShootingMethodUI(bool bIsSemiAutomatic,bool Hidden);
	void HiddenShootingMethodUI();
	FORCEINLINE class UQLPlayerHUDWidget* GetStatHUD() { return CharacterStat; }
	FORCEINLINE class UQLPlayerHpBarWidget* GetHpHUD() { return HpBar; }
protected:
	virtual void NativeConstruct() override;
	FTimerHandle HiddenTimer;

protected:

	UPROPERTY()
	TObjectPtr<class UQLPlayerHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class UQLPlayerHUDWidget> CharacterStat;

	UPROPERTY()
	TObjectPtr<class UQLSwitchWeaponWidget> SwitchWeapon;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtShootingMethod;

	uint8 bIsSemiAutomatic : 1;

};
