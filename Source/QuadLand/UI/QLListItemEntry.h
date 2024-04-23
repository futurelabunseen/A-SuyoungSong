// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "QLListItemEntry.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLListItemEntry : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	/** Follows the same pattern as the NativeOn[X] methods in UUserWidget - super calls are expected in order to route the event to BP. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//아이템 UI 설정 시 호출하는 함수

protected:

	UPROPERTY()
	TObjectPtr<class UImage> ItemImg;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtItemTitle;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtItemCnt;

};
