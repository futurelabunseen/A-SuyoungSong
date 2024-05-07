// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLAlertPanel.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAlertPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetStatusTxt(const FString& ItemName);

	void SetNickNameTxt(const FString& InNickname);
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Status;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NickName;

};
