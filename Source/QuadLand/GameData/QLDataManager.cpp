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

void UQLDataManager::PrintItemList()
{
	if (ItemDataManager)
	{
		for (const auto& ItemData : ItemDataManager->ItemList)
		{
			UE_LOG(LogTemp, Log, TEXT("%d"), ItemData->ItemType);
		}
	}
}

void UQLDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
