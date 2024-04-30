// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLItemData.h"
#include "QLAmmoData.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAmmoData : public UQLItemData, public IQLGetItemStat
{
	GENERATED_BODY()
	
public:
	UQLAmmoData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoCnt; //MaxAmmoCnt ���� ���� 
	
	virtual float GetStat() override;
};
