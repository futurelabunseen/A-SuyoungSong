// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLWeaponStat.generated.h"

/**
 * 
 */
UENUM()
enum class EWeaponType : uint8
{
	TypeA,
	TypeB,
	TypeC,
	SpecialTypeA,
	SpecialTypeB
};


UCLASS()
class QUADLAND_API UQLWeaponStat : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	EWeaponType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	TSoftObjectPtr<class UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoCnt;

};
