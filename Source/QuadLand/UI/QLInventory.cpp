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
		//Text����
		Entry->CurrentItemCnt++; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
		ItemList->RegenerateAllEntries(); //�� ī��Ʈ�� ���ؼ� �ٽ� ������Ʈ �ض�
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{

}
