// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLDataManager.h"
#include "GameData/QLItemDataset.h"
#include "GameData/QLWeaponDataset.h"
#include "GameData/QLItemData.h"

UQLDataManager::UQLDataManager()
{
	static ConstructorHelpers::FObjectFinder<UQLItemDataset> ItemDataManagerClassRef(TEXT("/Script/QuadLand.QLItemDataset'/Game/QuadLand/GameData/ItemList/DAQL_ItemDataset.DAQL_ItemDataset'"));

	if (ItemDataManagerClassRef.Object)
	{
		ItemDataManager = ItemDataManagerClassRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UQLWeaponDataset> WeaponDataManagerClassRef(TEXT("/Script/QuadLand.QLWeaponDataset'/Game/QuadLand/GameData/ItemList/DAQL_WeaponDataset.DAQL_WeaponDataset'"));

	if (WeaponDataManagerClassRef.Object)
	{
		WeaponStatManager = WeaponDataManagerClassRef.Object;
	}
}

const UQLWeaponStat* UQLDataManager::GetWeaponStat(ECharacterAttackType AttackType)
{
	return WeaponStatManager->GetWeaponStat(AttackType);
}

UQLItemData* UQLDataManager::GetItem(EItemType ItemId)
{
	return ItemDataManager->GetItem(ItemId);
}

TSubclassOf<class AQLItemBox> UQLDataManager::GetItemBoxClass(EItemType ItemId)
{
	return ItemDataManager->GetItemBoxClass(ItemId);
}

void UQLDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}