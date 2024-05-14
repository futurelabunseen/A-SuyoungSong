// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemData.h"
#include "GameData/WeaponType.h"
#include "Engine/DataAsset.h"
#include "QLWeaponStat.generated.h"

/**
 * ��� ���߿� Attribute Set���ε� �����س�����. �׸��� �ֿ��� �� �ش� ������ Reset �ǵ��� ���� !
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
	float AmmoCnt; //ó�� ������ �� �ִ� �Ѿ� ���� 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	//�ٴڿ� �ִ� Weapon�� ���� �⺻ ���� 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLItemBox> GroundWeapon;
};
