// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLLoadingPanel.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLLoadingPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTxtRemainingTime(uint32 Time);

protected:
	virtual void NativeConstruct() override;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TxtRemainingTime;
};
