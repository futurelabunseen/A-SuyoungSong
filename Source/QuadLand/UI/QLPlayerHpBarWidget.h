// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "QLPlayerHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLPlayerHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQLPlayerHpBarWidget(const FObjectInitializer& ObjectInitializer);

	void UpdateHPPercentage(float InHP,float InMaxHP);
	void UpdateStaminaPercentage(float InStamina, float InMaxStamina);
protected:

	virtual void NativeConstruct() override;

protected:

	UPROPERTY()
	TObjectPtr<class UProgressBar> HP;
	UPROPERTY()
 	TObjectPtr<class UProgressBar> Stamina;

	float CurrentStamina;
	float MaxStamina;

	float CurrentHP;

	float MaxHP;
};
