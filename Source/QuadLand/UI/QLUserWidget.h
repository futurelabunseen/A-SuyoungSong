// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void UpdateRemainingAmmo(float InAmmo);
	void ChangedHPPercentage(float InHP,float InMaxHP);

	FORCEINLINE class UQLPlayerHUDWidget* GetStatHUD() { return CharacterStat; }
	FORCEINLINE class UQLPlayerHpBarWidget* GetHpHUD() { return HpBar; }
protected:
	virtual void NativeConstruct() override;

protected:

	UPROPERTY()
	TObjectPtr<class UQLPlayerHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class UQLPlayerHUDWidget> CharacterStat;
};
