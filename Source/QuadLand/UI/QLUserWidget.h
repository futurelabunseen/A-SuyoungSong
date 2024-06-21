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

	void BlinkBag();
	void ConcealLifeStone(bool InVisible);

	void UpdateEquipWeaponUI(bool InVisible);
	void UpdateEquipBombUI(bool InVisible);

	FORCEINLINE class UQLPlayerHUDWidget* GetStatHUD() { return CharacterStat; }
	FORCEINLINE class UQLPlayerHpBarWidget* GetHpHUD() { return HpBar; }

	void UpdateLivePlayer(int16 InLivePlayer);
	void UpdateProgressTime(const FString &InTime);

	void SettingNickname(const FString& InNickname);

	void SettingStoneImg(class UTexture2D* StoneImg);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IMGContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IMGStone;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> Blink;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtShootingMethod;

	UPROPERTY(meta = (BindWiget))
	TObjectPtr<class UTextBlock> TxtProgressTime;

	UPROPERTY(meta = (BindWiget))
	TObjectPtr<class UTextBlock> TxtPersonCnt;

	UPROPERTY(meta = (BindWiget))
	TObjectPtr<class UTextBlock> TxtNickname;


	uint8 bIsSemiAutomatic : 1;

};
