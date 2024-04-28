// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLInventory.h"
#include "Player/QLPlayerController.h"
#include "Components/ListView.h"
#include "GameData/QLItemData.h"


UQLInventory::UQLInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetKeyboardFocus();
	bIsFocusable = true;
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

void UQLInventory::OnClickedItem()
{
	UQLItemData *ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());

	if (ItemEntry)
	{
		//해당 플레이어 컨트롤러에게 id-cnt값 전달
		//플레이어 컨트롤러는 PlayerPawn 전달 -> ServerRPC -> id/cnt 전송
		//현재 id 있는지
		//현재 character id - cnt 개수가 일치하는지 확인
		//Implementation -> 아이템 사용 -> cnt 개수 차감
		//해당 id - cnt 개수 하나 차감 -> ClientRPC 전송
		//Inventory Update
		AQLPlayerController* PC=CastChecked<AQLPlayerController>( GetOwningPlayer() );
		PC->RemoveItemEntry(ItemEntry->ItemType, ItemEntry->CurrentItemCnt);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{

}
