// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "QLPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQLPlayerHUDWidget(const FObjectInitializer& ObjectInitializer);

	void UpdateAmmo(float AmmoCnt);
	void UpdateRemainingAmmo(float AmmoCnt);

protected:
	virtual void NativeConstruct() override;

protected:
	
	UPROPERTY()
	TObjectPtr<class UTextBlock> CurrentAmmoCntTxt;

	UPROPERTY()
	TObjectPtr<class UTextBlock> RemainingAmmoCntTxt;

private:
	int CurrentAmmoCnt;
	int RemainingAmmoCnt;
};
