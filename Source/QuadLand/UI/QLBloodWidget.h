// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLBloodWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLBloodWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UQLBloodWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	void BlinkWidget();

	void CancelWidget();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UImage> BloodImage;

};
