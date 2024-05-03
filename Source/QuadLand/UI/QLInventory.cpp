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
			Entry->CurrentItemCnt = InItemCnt; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
			break;
		}
	}

	if (IsNotFound == true)
	{
		//생성
		AddItem(InItem);
	}
	
	ItemList->RegenerateAllEntries();
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
			Entry->CurrentItemCnt++; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
			break;
		}
	}

	if (IsNotFound == true)
	{
		//생성
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
}


void UQLInventory::OnClickedItem()
{
	UQLItemData* ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());

	if (ItemEntry)
	{
		//해당 플레이어 컨트롤러에게 id-cnt값 전달
		//플레이어 컨트롤러는 PlayerPawn 전달 -> ServerRPC -> id/cnt 전송
		//현재 id 있는지
		//현재 character id - cnt 개수가 일치하는지 확인
		//Implementation -> 아이템 사용 -> cnt 개수 차감
		//해당 id - cnt 개수 하나 차감 -> ClientRPC 전송
		//Inventory Update
		AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());
		PC->RemoveItemEntry(ItemEntry->ItemType, ItemEntry->CurrentItemCnt);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{
	ItemList->RemoveItem(Item);
}