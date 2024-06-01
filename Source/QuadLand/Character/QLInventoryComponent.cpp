// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLInventoryComponent.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLDataManager.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLItemData.h"
#include "Physics/QLCollision.h"
#include "UI/QLUserWidget.h"
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
		InventoryItem[ItemId] += ItemCnt; //없으면 0을 리턴
	}
	else
	{
		InventoryItem.Add(ItemId, ItemCnt);
	}

	QL_SUBLOG(QLNetLog, Warning, TEXT("Item Add %d"), InventoryItem[ItemId]);
}

void UQLInventoryComponent::UseItem(EItemType ItemId)
{

	if (EItemType::Ammo == ItemId || EItemType::Bomb == ItemId)
	{
		return; //얘는 사용할 수 없어;;
	}

	if (InventoryItem.Find(ItemId))
	{
		ServerRPCRemoveItem(ItemId, InventoryItem[ItemId]);
	}
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
	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	IQLGetItemStat* ItemStat = Cast<IQLGetItemStat>(ItemData);

	if (DataManager == nullptr)
	{
		return;
	}
	int32 ItemCnt = --InventoryItem[InItemId]; //하나 사용

	if (ItemStat)
	{
		switch (InItemId)
		{
		case EItemType::StaminaRecoveryItem:
			PS->AddStaminaStat(ItemStat->GetStat());
			MulticastRPCItemMotion(InItemId);
			//모션을 넣고싶음..
			break;
		case EItemType::HPRecoveryItem:
			PS->AddHPStat(ItemStat->GetStat());
			break;
		case EItemType::DiscoveryItem:
			PS->UseGlassesItem(ItemStat->GetStat());
			break;
		}
	}
	ClientRPCRemoveItem(InItemId, ItemCnt); //클라랑 서버랑 개수 일치
}

void UQLInventoryComponent::ClientRPCRemoveItem_Implementation(EItemType InItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	InventoryItem[InItemId] = ItemCnt;
	ItemData->CurrentItemCnt = ItemCnt;

	if (InItemId == EItemType::Bomb && InventoryItem[InItemId] <= 0.0f)
	{
		PC->UpdateEquipBombUI();
	}
	PC->UpdateItemEntry(ItemData, ItemCnt);
}


void UQLInventoryComponent::AddInventoryByDraggedItem(EItemType InItemId, int32 InItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	if (!HasAuthority())
	{
		AddItem(InItemId,InItemCnt);
	}
	//실제로 아이템이 있는지 검사하기 위해서 서버에게 요청해야함;
	PC->BlinkBag();
	ServerRPCAddInventoryByDraggedItem(InItemId, InItemCnt);

	if (InItemId == EItemType::Bomb && GetInventoryCnt(InItemId) <= 1.0f)
	{
		PC->UpdateEquipBombUI();
	}
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
	//서버에서만 적용
	FCollisionQueryParams Params(TEXT("DetectionItem"), false, Character);

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
				//인벤토리에 Item 정보를 전송
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
				if (ItemData->ItemType == ItemId)
				{
					HitItem->SetLifeSpan(0.3f);
					//같으면 추가한다.
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
		ClientRPCRollbackInventory(ItemId, ItemCnt);
	}
}

void UQLInventoryComponent::AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt)
{

	if (!HasAuthority())
	{
		if (InventoryItem.Find(ItemId))
		{
			InventoryItem[ItemId] -= ItemCnt;
		}
		else
		{
			return;
		}
	}
	//Server RPC 전송 -> Server 아이템 생성 및 아이템 조정 
	ServerRPCAddGroundByDraggedItem(ItemId, ItemCnt);
}

void UQLInventoryComponent::MulticastRPCItemMotion_Implementation(EItemType ItemId)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	switch (ItemId)
	{
		case EItemType::StaminaRecoveryItem:
			Character->PlayAnimMontage(ItemMontage[ItemId]); //Stand
		break;
	}
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
	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	//Data매니저가 가지고 있는 초기값 만큼 Stat을 없앰
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = Character->GetActorLocation();
		FActorSpawnParameters Params;
		Params.Owner = Character;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);
		GroundItem->SetPhysics();

		if (ItemId == EItemType::Ammo)
		{
			IQLGetItemStat* AmmoStatData = CastChecked<IQLGetItemStat>(DataManager->GetItem(ItemId));
			AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
			if (PS == nullptr)
			{
				return;
			}
			PS->BulletWaste(AmmoStatData->GetStat()); //땅에 버릴 때 
		}
		//	GroundItem->bReplicate
	}
}

void UQLInventoryComponent::ClientRPCAddItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	UQLItemData* ItemData = DataManager->GetItem(ItemId);
	if (GetNetMode() == ENetMode::NM_Client)
	{
		ItemData->CurrentItemCnt = GetInventoryCnt(ItemId) + ItemCnt;
		AddItem(ItemId, ItemCnt);
	}
	PC->UpdateItemEntry(ItemData, ItemData->CurrentItemCnt);
	PC->BlinkBag();
}

void UQLInventoryComponent::ClientRPCRollbackInventory_Implementation(EItemType InItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(InItemId))
	{
		InventoryItem[InItemId] -= ItemCnt;
	}
}