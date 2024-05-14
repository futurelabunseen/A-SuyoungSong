// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemData.h"
#include "GameData/WeaponType.h"
#include "Engine/DataAsset.h"
#include "QLWeaponStat.generated.h"

/**
 * 얘네 나중에 Attribute Set으로도 설정해놔야함. 그리고 주웠을 때 해당 값으로 Reset 되도록 구현 !
 */

UCLASS()
class QUADLAND_API UQLWeaponStat : public UQLItemData
{
	GENERATED_BODY()
	
public:
	UQLWeaponStat();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("QLItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	EWeaponType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoCnt; //처음 장전할 수 있는 총알 개수 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	//바닥에 있는 Weapon에 대한 기본 정보 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLItemBox> GroundWeapon;
};
