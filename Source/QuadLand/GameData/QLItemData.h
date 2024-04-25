// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLItemData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EItemType
{
	Weapon = 0,
	Stone,
	Ammo,
	StaminaRecoveryItem,
	HPRecoveryItem,
	DiscoveryItem,
	None
};

UCLASS()
class QUADLAND_API UQLItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("QLItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType ItemType;
};
