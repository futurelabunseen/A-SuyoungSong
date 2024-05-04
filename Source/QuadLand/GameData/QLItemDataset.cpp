// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLItemDataset.h"

UQLItemData* UQLItemDataset::GetItem(EItemType ItemId)
{
	return ItemList[ItemId];
}

TSubclassOf<class AQLItemBox> UQLItemDataset::GetItemBoxClass(EItemType ItemId)
{
	return GroundItemBox[ItemId];
}
