// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLInventoryComponent.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLDataManager.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLItemData.h"
#include "Physics/QLCollision.h"
#include "Item/QLItemBox.h"
#include "QuadLand.h"

UQLInventoryComponent::UQLInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsSetVisibleInventory = false;
}


void UQLInventoryComponent::AddItem(EItemType ItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(ItemId))
	{
		ItemCnt = ++InventoryItem[ItemId]; //������ 0�� ����
	}
	else
	{
		InventoryItem.Add(ItemId, ItemCnt);
	}
}

void UQLInventoryComponent::UseItem(EItemType ItemId)
{

	UE_LOG(LogTemp, Warning, TEXT("Use Item 1"));
	if (EItemType::Ammo == ItemId || EItemType::Bomb == ItemId)
	{
		UE_LOG(LogTemp, Warning, TEXT("Use Item 2"));
		return; //��� ����� �� ����;;
	}
	UE_LOG(LogTemp, Warning, TEXT("Use Item 3"));
	if (InventoryItem.Find(ItemId))
	{
		ServerRPCRemoveItem(ItemId, InventoryItem[ItemId]);
		UE_LOG(LogTemp, Warning, TEXT("Use Item 4"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Use Item 5"));
}

int UQLInventoryComponent::GetInventoryCnt(EItemType ItemType)
{
	if (InventoryItem.Find(ItemType) == 0)
		return 0;
	return InventoryItem[ItemType];
}
bool UQLInventoryComponent::ServerRPCRemoveItem_Validate(EItemType ItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(ItemId) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is currently exceeded"));
		return false;
	}

	if (InventoryItem[ItemId] != ItemCnt)
	{
		UE_LOG(LogTemp, Warning, TEXT("The number of items does not match. %d %d"), InventoryItem[ItemId], ItemCnt);
		return false;
	}
	return true;
}

void UQLInventoryComponent::ServerRPCRemoveItem_Implementation(EItemType InItemId, int32 InItemCnt)
{
	UE_LOG(LogTemp, Warning, TEXT("found a matching item %d %d"), InItemId, InventoryItem[InItemId]);

	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	if (PS == nullptr)
	{
		return;
	}
	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	//�������� �����ϰ� �ִ� Manager �����´�.

	//�̺κ� ���� �����ؼ� �����ϵ��� ���� ����.
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	IQLGetItemStat* ItemStat = Cast<IQLGetItemStat>(ItemData);

	if (DataManager == nullptr)
	{
		return;
	}
	if (ItemStat)
	{
		switch (InItemId)
		{
		case EItemType::StaminaRecoveryItem:
			PS->AddStaminaStat(ItemStat->GetStat());
			break;
		case EItemType::HPRecoveryItem:
			PS->AddHPStat(ItemStat->GetStat());
			break;
		case EItemType::DiscoveryItem:
			PS->UseGlassesItem(ItemStat->GetStat());
			break;
		}
	}
	int32 ItemCnt = --InventoryItem[InItemId]; //�ϳ� ���

	UE_LOG(LogTemp, Warning, TEXT("item use %d %d"), InItemId, InventoryItem[InItemId]);

	ClientRPCRemoveItem(ItemData, ItemCnt); //Ŭ��� ������ ���� ��ġ
}

void UQLInventoryComponent::ClientRPCRemoveItem_Implementation(UQLItemData* Item, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}
	InventoryItem[Item->ItemType] = ItemCnt;
	Item->CurrentItemCnt = ItemCnt;
	PC->UpdateItemEntry(Item, ItemCnt);
	UE_LOG(LogTemp, Warning, TEXT("update? %d %d"), Item->ItemType, InventoryItem[Item->ItemType]);

}


void UQLInventoryComponent::AddInventoryByDraggedItem(EItemType InItemId, int32 InItemCnt)
{
	if (!HasAuthority())
	{
		if (InventoryItem.Find(InItemId))
		{
			InventoryItem[InItemId] += InItemCnt;
			QL_SUBLOG(QLNetLog, Warning, TEXT("Same Item %d"), InItemCnt);
		}
		else
		{
			InventoryItem.Add(InItemId, InItemCnt);
		}

	}
	//������ �������� �ִ��� �˻��ϱ� ���ؼ� �������� ��û�ؾ���;
	ServerRPCAddInventoryByDraggedItem(InItemId, InItemCnt);
	QL_SUBLOG(QLNetLog, Warning, TEXT("AddItem %d"), InItemCnt);
}

void UQLInventoryComponent::ServerRPCAddInventoryByDraggedItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (PC == nullptr|| Character == nullptr)
	{
		return;
	}
	bool bResult = false;
	FVector SearchLocation = Character->GetMesh()->GetSocketLocation(FName("ItemDetectionSocket"));
	//���������� ����
	FCollisionQueryParams Params(TEXT("DetectionItem"), false, Character);

	UE_LOG(LogTemp, Warning, TEXT("Add Item %d"), ItemCnt);

	TArray<FHitResult> NearbyItems;
	//ItemDetectionSocket
	bResult = GetWorld()->SweepMultiByChannel(
		NearbyItems,
		SearchLocation,
		SearchLocation,
		FQuat::Identity,
		CCHANNEL_QLITEMACTION,
		FCollisionShape::MakeSphere(Character->SearchRange),
		Params
	);

	if (bResult)
	{
		bool IsNotFound = true;
		for (const auto& NearbyItem : NearbyItems)
		{
			AQLItemBox* HitItem = Cast<AQLItemBox>(NearbyItem.GetActor());
			if (HitItem)
			{
				//�κ��丮�� Item ������ ����
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
				if (ItemData->ItemType == ItemId)
				{
					HitItem->SetLifeSpan(0.3f);
					//������ �߰��Ѵ�.
					if (InventoryItem.Find(ItemId))
					{
						IsNotFound = false;
						InventoryItem[ItemId]++;
						UE_LOG(LogTemp, Log, TEXT("Current Cnt %d"), InventoryItem[ItemId]);
					}

					if (ItemId == EItemType::Ammo)
					{
						AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
						if (PS == nullptr)
						{
							return;
						}
						IQLGetItemStat* ItemStat = CastChecked<IQLGetItemStat>(ItemData);
						PS->SetAmmoStat(ItemStat->GetStat());
					}
				}
			}
		}

		if (IsNotFound)
		{
			InventoryItem.Add(ItemId, ItemCnt);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found, Rollback"), ItemCnt);
		ClientRPCRollbackInventory(ItemId, ItemCnt);
	}


#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SearchLocation, Character->SearchRange, 10.0f, Color, false, 5.0f);
#endif
}

void UQLInventoryComponent::AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt)
{
	UE_LOG(LogTemp, Warning, TEXT("AddGroundByDraggedItem %d"), ItemCnt);

	if (!HasAuthority())
	{
		if (InventoryItem.Find(ItemId))
		{
			InventoryItem[ItemId] -= ItemCnt;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error / is not valid %d"), ItemCnt);
			return;
		}
	}
	//Server RPC ���� -> Server ������ ���� �� ������ ���� 
	ServerRPCAddGroundByDraggedItem(ItemId, ItemCnt);
}

void UQLInventoryComponent::ServerRPCAddGroundByDraggedItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();

	if (Character == nullptr)
	{
		return;
	}
	int RemainingItemCnt = 0;
	if (InventoryItem.Find(ItemId))
	{
		RemainingItemCnt = InventoryItem[ItemId];
		InventoryItem[ItemId] -= ItemCnt;
		UE_LOG(LogTemp, Warning, TEXT("Current Server Item Cnt %d"), InventoryItem[ItemId]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error / is not valid %d"), ItemCnt);
		return;
	}

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	//�������� �����ϰ� �ִ� Manager �����´�.

	//�̺κ� ���� �����ؼ� �����ϵ��� ���� ����.
	//Data�Ŵ����� ������ �ִ� �ʱⰪ ��ŭ Stat�� ����
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = Character->GetActorLocation();
		Location.X += 5.0f * i;
		FActorSpawnParameters Params;
		Params.Owner = Character;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);

		if (ItemId == EItemType::Ammo)
		{
			IQLGetItemStat* AmmoStatData = CastChecked<IQLGetItemStat>(DataManager->GetItem(ItemId));
			AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
			if (PS == nullptr)
			{
				return;
			}
			PS->BulletWaste(AmmoStatData->GetStat()); //���� ���� �� 
		}
		UE_LOG(LogTemp, Warning, TEXT("AddGroundByDraggedItem %d"), ItemCnt);
		//	GroundItem->bReplicate
	}
}
void UQLInventoryComponent::ClientRPCAddItem_Implementation(UQLItemData* ItemData, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	if (!InventoryItem.Find(ItemData->ItemType))
	{
		InventoryItem.Add(ItemData->ItemType, ItemCnt);
	}
	else
	{
		InventoryItem[ItemData->ItemType] = ItemCnt;
	}

	ItemData->CurrentItemCnt = ItemCnt;
	PC->UpdateItemEntry(ItemData, ItemCnt);
}

void UQLInventoryComponent::ClientRPCRollbackInventory_Implementation(EItemType InItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(InItemId))
	{
		InventoryItem[InItemId] -= ItemCnt;
	}
}