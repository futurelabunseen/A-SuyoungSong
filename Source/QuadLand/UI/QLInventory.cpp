// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLInventory.h"
#include "Player/QLPlayerController.h"
#include "Components/ListView.h"
#include "UI/QLListItemEntry.h"
#include "GameData/QLItemData.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

UQLInventory::UQLInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetKeyboardFocus();
	bIsFocusable = true;
}

void UQLInventory::AddItem(UObject* Item)
{
	ItemList->AddItem(Item);
}

void UQLInventory::UpdateItemEntry(UObject* InItem, int32 InItemCnt)
{
	const TArray<UObject*> Items = ItemList->GetListItems();
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem);
	bool IsNotFound = true;
	for (const auto& Item : Items)
	{
		UQLItemData* Entry = Cast<UQLItemData>(Item);

		UE_LOG(LogTemp, Warning, TEXT("?"));
		if (Entry && Entry->ItemType == InItemInfo->ItemType)
		{
			IsNotFound = false;
			if (InItemCnt <= 0)
			{
				DeleteItem(Entry);
				break;
			}
			Entry->CurrentItemCnt = InItemCnt; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
			break;
		}
	}

	if (IsNotFound == true)
	{
		//����
		AddItem(InItem);
	}
	
	ItemList->RegenerateAllEntries();
}

void UQLInventory::UpdateInventoryByDraggedItem(UObject* InItem)
{
	
	AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());

	const TArray<UObject*> Items = ItemList->GetListItems();
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem); //Entry - InItemInfo ����Ǿ��־, 2��� �����ϴ� ������ �߻�����. (������ �ּҰ� ���Ƽ� �׷���)
	bool IsNotFound = true;

	int AddedItemCnt = InItemInfo->CurrentItemCnt;
	for (const auto& Item : Items)
	{
		UQLItemData* Entry = Cast<UQLItemData>(Item);

		if (Entry && Entry->ItemType == InItemInfo->ItemType)
		{
			IsNotFound = false;
			Entry->CurrentItemCnt += AddedItemCnt; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
			
			break;
		}
	}

	//���ϱ� �̸鼭

	if (IsNotFound == true)
	{
		//����
		ItemList->AddItem(InItem);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("InItemInfo->CurrentItemCnt %d"), AddedItemCnt);
	//���� ������ ������ �����ؾ���.
	PC->AddInventoryByDraggedItem(InItemInfo->ItemType, AddedItemCnt);
	ItemList->RegenerateAllEntries();
}

void UQLInventory::UpdateNearbyItemEntryByDraggedItem(UObject* InItem)
{
	GroundItem->AddItem(InItem);
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem);
	//PCȣ��
	AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());
	PC->AddGroundByDraggedItem(InItemInfo->ItemType, InItemInfo->CurrentItemCnt);
}


void UQLInventory::UpdateNearbyItemEntry(UObject* InItem)
{
	/**/
	const TArray<UObject*> Items = GroundItem->GetListItems();
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem);
	bool IsNotFound = true;
	for (const auto& Item : Items)
	{
		UQLItemData* Entry = Cast<UQLItemData>(Item);

		if (Entry && Entry->ItemType == InItemInfo->ItemType)
		{
			IsNotFound = false;
			Entry->CurrentItemCnt++; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
			break;
		}
	}

	if (IsNotFound == true)
	{
		//����
		GroundItem->AddItem(InItem);
	}
}

void UQLInventory::RemoveNearbyItemEntry(UObject* InItem)
{
	GroundItem->RemoveItem(InItem);
}

void UQLInventory::RemoveAllNearbyItemEntries()
{
	GroundItem->ClearListItems();
	GroundItem->RegenerateAllEntries();
}

void UQLInventory::OnClickedItem()
{
	UQLItemData* ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());

	UE_LOG(LogTemp, Warning, TEXT("%d"),ItemEntry);
	if (ItemEntry)
	{
		//�ش� �÷��̾� ��Ʈ�ѷ����� id-cnt�� ����
		//�÷��̾� ��Ʈ�ѷ��� PlayerPawn ���� -> ServerRPC -> id/cnt ����
		//���� id �ִ���
		//���� character id - cnt ������ ��ġ�ϴ��� Ȯ��
		//Implementation -> ������ ��� -> cnt ���� ����
		//�ش� id - cnt ���� �ϳ� ���� -> ClientRPC ����
		//Inventory Update
		AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());
		PC->RemoveItemEntry(ItemEntry->ItemType);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{
	ItemList->RemoveItem(Item);
}