// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/WeaponType.h"
#include "QLSwitchWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLSwitchWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//총을 획득하거나, 사용중일 때 SwitchWeaponWidget 전달
	UQLSwitchWeaponWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	void SwitchWeaponStyle(ECharacterAttackType AttackType);
	void UpdateEquipWeaponUI(bool bIsVisible); //Gun UI 등장
	void UpdateEquipBombUI(bool bIsVisible); //Bomb UI 등장 
protected:

	//UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	//TObjectPtr<class UWidgetAnimation> WeaponFadeIn;

	//UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	//TObjectPtr<class UWidgetAnimation> BomnFadeIn;

	FVector2D DeactivePunchScale;
	FVector2D ActivePunchScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IMGPunch;

	FVector2D DeactiveGunScale;
	FVector2D ActiveGunScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IMGGun;

	FVector2D DeactiveBombScale;
	FVector2D ActiveBombScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IMGBomb;

	FLinearColor ActiveColor;
	FLinearColor DiactiveColor;
};
