// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemType.h"
#include "Engine/DataAsset.h"
#include "QLItemData.generated.h"

/**
 * 
 */

UCLASS()
class QUADLAND_API UQLItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("QLItemData", GetFName());
	}
	UQLItemData();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<class UTexture2D> ItemImg;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString ItemName;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentItemCnt; //변화하는 값, 플레이어마다 다른 값을 가짐

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, Meta = (AllowPrivateAccess = "true"))
	uint8 bIsGround : 1; //변화하는 값, 플레이어마다 다른 값을 가짐
};
