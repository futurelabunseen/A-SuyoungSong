// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLListItemEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UQLListItemEntry::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImg = Cast<UImage>(GetWidgetFromName(TEXT("ItemImg")));

	TxtItemTitle = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtItemTitle")));

	TxtItemCnt = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtItemCnt")));
}

void UQLListItemEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	
}
