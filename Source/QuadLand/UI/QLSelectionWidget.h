// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ChangeGenderTypeToInt(FName Gender);

	UFUNCTION(BlueprintCallable)
	void ChangeGemTypeToInt(FName GemColor);

	UFUNCTION(BlueprintCallable)
	void SetReady();

private:
	int Gender= 0;
	int Gem = 0;
};
