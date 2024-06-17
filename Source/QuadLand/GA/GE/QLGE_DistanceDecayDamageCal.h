// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "QLGE_DistanceDecayDamageCal.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGE_DistanceDecayDamageCal : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:

	UPROPERTY(EditAnywhere, Category = BoneName)
	FName DamageBaseBone;


};
