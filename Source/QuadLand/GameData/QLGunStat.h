// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLWeaponStat.h"
#include "QLGunStat.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGunStat : public UQLWeaponStat
{
	GENERATED_BODY()

public:
	UQLGunStat();

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<class USkeletalMesh> WeaponMesh;
	
};
