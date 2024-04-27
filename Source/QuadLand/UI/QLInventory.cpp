// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLInventory.h"
#include "Components/ListView.h"
#include "GameData/QLItemData.h"

void UQLInventory::NativeConstruct()
{
}

void UQLInventory::AddItem(UObject* Item)
{
	ItemList->AddItem(Item);
}

void UQLInventory::UpdateItemEntry(UObject* Item,int32 InItemCnt)
{
	int32 ItemIdx = ItemList->GetIndexForItem(Item);
	UE_LOG(LogTemp, Log, TEXT("%d"), ItemIdx);

	UQLItemData *Entry = Cast<UQLItemData>(ItemList->GetItemAt(ItemIdx));
	if (Entry)
	{
		//Text변경
		Entry->CurrentItemCnt++; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
		ItemList->RegenerateAllEntries(); //그 카운트에 대해서 다시 업데이트 해라
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{

}
