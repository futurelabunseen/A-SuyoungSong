// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLNicknameWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLNicknameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetNickname(FString InNickname);
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtNickname;
};
