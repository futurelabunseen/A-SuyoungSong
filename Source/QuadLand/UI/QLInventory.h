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

public:
	UQLInventory(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(meta = (BindWidget)) //자동으로 연결되나봄..!
	TObjectPtr<class UListView> ItemList;


	void AddItem(UObject* Item);
	void UpdateItemEntry(UObject* Item, int32 InItemCnt);

	UFUNCTION(BlueprintCallable)
	void OnClickedItem();
	
	void DeleteItem(UObject* Item);

};
