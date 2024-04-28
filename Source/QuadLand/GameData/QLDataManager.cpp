// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLDataManager.h"
#include "GameData/QLItemDataset.h"
#include "GameData/QLItemData.h"

UQLDataManager::UQLDataManager()
{
	static ConstructorHelpers::FObjectFinder<UQLItemDataset> ItemDataManagerClassRef(TEXT("/Script/QuadLand.QLItemDataset'/Game/QuadLand/GameData/ItemList/DAQL_ItemDataset.DAQL_ItemDataset'"));

	if (ItemDataManagerClassRef.Object)
	{
		ItemDataManager = ItemDataManagerClassRef.Object;
	}
}

UQLItemData* UQLDataManager::GetItem(EItemType ItemId)
{
	return ItemDataManager->GetItem(ItemId);
}

void UQLDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
