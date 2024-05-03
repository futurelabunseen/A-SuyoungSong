// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLAmmoData.h"
#include "QLAmmoData.h"

UQLAmmoData::UQLAmmoData()
{
	UQLItemData::ItemType = EItemType::Ammo;
}

float UQLAmmoData::GetStat()
{
	return AmmoCnt;
}
