// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLDeathTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDeathTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void UpdateTimer(int Time); //타이머 10초간 업데이트

	//void SetTxt(); //static ? 일단해보고 수정해봐
	//void ClearTimer();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtTimer;
};
