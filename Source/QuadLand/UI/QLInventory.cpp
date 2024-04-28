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
		//Text����
		Entry->CurrentItemCnt++; //�κ��丮�� �������� ������, �� �������� �����ͼ� ī��Ʈ�� ������Ű��
		ItemList->RegenerateAllEntries(); //�� ī��Ʈ�� ���ؼ� �ٽ� ������Ʈ �ض�
	}
}

void UQLInventory::OnClickedItem()
{
	UQLItemData *ItemEntry = Cast<UQLItemData>(ItemList->GetSelectedItem());

	if (ItemEntry)
	{
		//�ش� �÷��̾� ��Ʈ�ѷ����� id-cnt�� ����
		//�÷��̾� ��Ʈ�ѷ��� PlayerPawn ���� -> ServerRPC -> id/cnt ����
		//���� id �ִ���
		//���� character id - cnt ������ ��ġ�ϴ��� Ȯ��
		//Implementation -> ������ ��� -> cnt ���� ����
		//�ش� id - cnt ���� �ϳ� ���� -> ClientRPC ����
		//Inventory Update
		AQLPlayerController* PC=CastChecked<AQLPlayerController>( GetOwningPlayer() );
		PC->RemoveItemEntry(ItemEntry->ItemType, ItemEntry->CurrentItemCnt);
		UE_LOG(LogTemp, Warning, TEXT("Current Selected"));
	}
}

void UQLInventory::DeleteItem(UObject* Item)
{

}
