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

	UPROPERTY(meta = (BindWidget)) //�ڵ����� ����ǳ���..!
	TObjectPtr<class UListView> ItemList; //���� ���� ������ �ִ� �������� �����ִ� UI
	
	UPROPERTY(meta = (BindWidget)) 
	TObjectPtr<class UListView> GroundItem; //�ֺ� ������ UI

	void AddItem(UObject* Item);
	void UpdateItemEntry(UObject* Item, int32 InItemCnt);

	void AddNearbyItemEntry(UObject* Item);
	void RemoveNearbyItemEntry();
	
	UFUNCTION(BlueprintCallable)
	void OnClickedItem();
	
	void DeleteItem(UObject* Item);

};
