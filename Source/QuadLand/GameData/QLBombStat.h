// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLWeaponStat.h"
#include "GameData/WeaponType.h"
#include "QLBombStat.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLBombStat : public UQLWeaponStat
{
	GENERATED_BODY()
	
public:

	UQLBombStat();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("QLItemData", GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLBomb> BombClass;
};
