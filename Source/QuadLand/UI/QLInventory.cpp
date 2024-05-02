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

void UQLInventory::UpdateItemEntry(UObject* InItem,int32 InItemCnt)
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
			Entry->CurrentItemCnt = InItemCnt; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
			ItemList->RegenerateAllEntries(); //�� ī��Ʈ�� ���ؼ� �ٽ� ������Ʈ �ض�
			break;
		}
	}

	if (IsNotFound == true)
	{
		//����
		AddItem(InItem);
	}
}

void UQLInventory::AddNearbyItemEntry(UObject* Item)
{
	GroundItem->AddItem(Item);
}

void UQLInventory::RemoveNearbyItemEntry()
{
	GroundItem->ClearListItems();
}


void UQLInventory::OnClickedItem()
{
	UQLItemData* ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());

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
		PC->RemoveItemEntry(ItemEntry->ItemType, ItemEntry->CurrentItemCnt);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{
	ItemList->RemoveItem(Item);
}
