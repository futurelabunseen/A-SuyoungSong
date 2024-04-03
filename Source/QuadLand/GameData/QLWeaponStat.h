// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLWeaponStat.generated.h"

/**
 * 얘네 나중에 Attribute Set으로도 설정해놔야함. 그리고 주웠을 때 해당 값으로 Reset 되도록 구현 !
 */
UENUM()
enum class EWeaponType : uint8
{
	TypeA,
	TypeB,
	TypeC,
	SpecialTypeA,
	SpecialTypeB,
	None, //None은 Punch를 의미함.
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
