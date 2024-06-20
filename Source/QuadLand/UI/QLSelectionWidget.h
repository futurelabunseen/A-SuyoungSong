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

	UFUNCTION()
	void ShowWaitTxt();

	UFUNCTION()
	void ShowLimitTxt();

	UFUNCTION()
	void StartLimitTimer();
	
protected:

	FTimerHandle LimitTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	int Gender= 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	int Gem = 0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtWait;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeLimit")
	int TimeLimitSec;

	int StartTime;
	uint8 CheckTimeLimit : 1;
};
