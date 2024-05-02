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
	TObjectPtr<class UListView> ItemList; //현재 내가 가지고 있는 아이템을 보여주는 UI
	
	UPROPERTY(meta = (BindWidget)) 
	TObjectPtr<class UListView> GroundItem; //주변 아이템 UI

	void AddItem(UObject* Item);
	void UpdateItemEntry(UObject* Item, int32 InItemCnt);

	void AddNearbyItemEntry(UObject* Item);
	void RemoveNearbyItemEntry();
	
	UFUNCTION(BlueprintCallable)
	void OnClickedItem();
	
	void DeleteItem(UObject* Item);

};
