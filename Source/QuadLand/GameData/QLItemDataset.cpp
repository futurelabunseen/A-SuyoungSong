// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLItemDataset.h"

UQLItemData* UQLItemDataset::GetItem(int id)
{
	if (ItemList.Num() <= id)
	{
		return nullptr;
	}
	return ItemList[id];
}
