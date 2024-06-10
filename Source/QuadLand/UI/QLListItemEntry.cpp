// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLListItemEntry.h"
#include "GameData/QLItemData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UQLListItemEntry::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImg = Cast<UImage>(GetWidgetFromName(TEXT("ItemImage")));
	TxtItemTitle = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtItemTitle")));
	TxtItemCnt = Cast< UTextBlock>(GetWidgetFromName(TEXT("TxtItemCnt")));
}

void UQLListItemEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	const UQLItemData* Data = Cast<UQLItemData>(ListItemObject);
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Data is not valid"));
		return;
	}

	if (ItemImg && Data->ItemImg)
	{
		ItemImg->SetBrushFromTexture(Data->ItemImg);
	}
	//이친구,,, 어떻게가져와?
	FString ItemCntTxt = FString::Printf(TEXT("%d"), Data->CurrentItemCnt);
	TxtItemCnt->SetText(FText::FromString(ItemCntTxt));
	TxtItemTitle->SetText(FText::FromString(Data->ItemName));
}

void UQLListItemEntry::SetItemNameTxt(FString ItemName)
{
	TxtItemTitle->SetText(FText::FromString(ItemName));
}

void UQLListItemEntry::SetItemCntTxt(int32 ItemCnt)
{
	FString ItemCntTxt = FString::Printf(TEXT("%d"), ItemCnt);
	TxtItemCnt->SetText(FText::FromString(ItemCntTxt));
}