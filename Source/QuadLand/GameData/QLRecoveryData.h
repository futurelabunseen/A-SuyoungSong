// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemData.h"
#include "QLRecoveryData.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLRecoveryData : public UQLItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float RecoveryRate; //미니맵에 보여주는 시간 초 
};
