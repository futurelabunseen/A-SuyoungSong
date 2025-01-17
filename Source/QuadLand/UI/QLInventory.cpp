// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLInventory.h"
#include "Player/QLPlayerController.h"
#include "Character/QLCharacterPlayer.h"
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
	ItemList->RequestRefresh();
}

void UQLInventory::UpdateItemEntry(UObject* InItem, int32 InItemCnt)
{
	const TArray<UObject*> Items = ItemList->GetListItems();
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem);
	bool IsNotFound = true;

	for (const auto& Item : Items)
	{
		UQLItemData* Entry = Cast<UQLItemData>(Item);

		if (Entry && Entry->ItemType == InItemInfo->ItemType)
		{
			IsNotFound = false;
			if (InItemCnt <= 0)
			{
				DeleteItem(Entry);
				break;
			}
			Entry->CurrentItemCnt = InItemCnt; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
			UE_LOG(LogTemp, Warning, TEXT("Item Type %d Cnt %d %d"), InItemInfo->ItemType, InItemInfo->CurrentItemCnt, InItemCnt);

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

void UQLInventory::UpdateInventoryByDraggedItem(UObject* InItem)
{

	AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());
	AQLCharacterPlayer* Player = CastChecked<AQLCharacterPlayer>(PC->GetPawn());


	const TArray<UObject*> Items = ItemList->GetListItems();
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem); //Entry - InItemInfo 연결되어있어서, 2배로 증가하는 현상이 발생했음. (포인터 주소가 같아서 그런것)
	bool IsNotFound = true;

	int AddedItemCnt = InItemInfo->CurrentItemCnt;

	for (const auto& Item : Items)
	{
		UQLItemData* Entry = Cast<UQLItemData>(Item);

		if (Entry && Entry->ItemType == InItemInfo->ItemType)
		{
			IsNotFound = false;
			Entry->CurrentItemCnt = Player->GetInventoryCnt(InItemInfo->ItemType) + AddedItemCnt; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
			break;
		}
	}

	//더하기 이면서

	if (IsNotFound == true)
	{
		//생성
		ItemList->AddItem(InItem);
	}

	//현재 증가된 개수를 전달해야함.
	PC->AddInventoryByDraggedItem(InItemInfo->ItemType); //추가된 개수를 전달할 필요없음.
	ItemList->RegenerateAllEntries();
}

void UQLInventory::UpdateNearbyItemEntryByDraggedItem(UObject* InItem)
{
	//GroundItem->AddItem(InItem);
	UQLItemData* InItemInfo = Cast<UQLItemData>(InItem);
	//PC호출
	AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());
	PC->AddGroundByDraggedItem(InItemInfo->ItemType);
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
			Entry->CurrentItemCnt += 1; //인벤토리에 아이템이 있으면, 그 아이템을 가져와서 카운트를 증가시키고
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("UpdateNearbyItemEntry %d"), InItemInfo->CurrentItemCnt);

	if (IsNotFound == true)
	{
		//생성
		GroundItem->AddItem(InItem);
	}
	GroundItem->RegenerateAllEntries();
}

void UQLInventory::RemoveNearbyItemEntry(UObject* InItem)
{
	GroundItem->RemoveItem(InItem);
	GroundItem->RequestRefresh();
}

void UQLInventory::RemoveAllNearbyItemEntries()
{
	GroundItem->ClearListItems();
}

void UQLInventory::OnClickedItem()
{
	UQLItemData* ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());
	
	if (ItemEntry == nullptr)
	{
		return;
	}

	AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetOwningPlayer());

	UE_LOG(LogTemp, Warning, TEXT("%d"), ItemEntry);

	if (ItemEntry->ItemType == EItemType::HPRecoveryItem)
	{
		//소리 나온다.
		PC->PlayCanSound();
	}
	if (ItemEntry)
	{
		//해당 플레이어 컨트롤러에게 id-cnt값 전달
		//플레이어 컨트롤러는 PlayerPawn 전달 -> ServerRPC -> id/cnt 전송
		//현재 id 있는지
		//현재 character id - cnt 개수가 일치하는지 확인
		//Implementation -> 아이템 사용 -> cnt 개수 차감
		//해당 id - cnt 개수 하나 차감 -> ClientRPC 전송
		//Inventory Update
		PC->RemoveItemEntry(ItemEntry->ItemType);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{
	ItemList->RemoveItem(Item);
	ItemList->RequestRefresh();
}

void UQLInventory::ClearAll() //리스폰 되면서 아이템 모두 사라짐
{
	ItemList->ClearListItems();
	GroundItem->ClearListItems();
}