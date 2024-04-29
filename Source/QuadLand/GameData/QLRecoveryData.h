// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLItemData.h"
#include "QLRecoveryData.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLRecoveryData : public UQLItemData, public IQLGetItemStat
{
	GENERATED_BODY()

public:

	virtual float GetStat() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float RecoveryRate; //�̴ϸʿ� �����ִ� �ð� �� 
};
