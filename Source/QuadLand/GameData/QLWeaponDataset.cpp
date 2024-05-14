// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLWeaponDataset.h"

const UQLWeaponStat* UQLWeaponDataset::GetWeaponStat(ECharacterAttackType AttackType)
{
	return WeaponStatManager[AttackType];
}
