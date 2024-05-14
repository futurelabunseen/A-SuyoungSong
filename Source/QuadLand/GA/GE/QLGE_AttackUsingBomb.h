// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "QLGE_AttackUsingBomb.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGE_AttackUsingBomb : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat)
	float AddBombDamage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stat)
	float AddBombRadius;
};
