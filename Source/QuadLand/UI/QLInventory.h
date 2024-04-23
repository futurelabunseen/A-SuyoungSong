// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLInventory.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLInventory : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;

public:

	UPROPERTY()
	TArray<TObjectPtr<class UQLListItemEntry>> Items;
};
