// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "QLTA_TraceResult.generated.h"

/**
 * 
/**
 *
 */
UCLASS()
class QUADLAND_API AQLTA_TraceResult : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:

	/** Initialize and begin targeting logic  */
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	/** Requesting targeting data, but not necessarily stopping/destroying the task. Useful for external target data requests. */
	virtual void ConfirmTargetingAndContinue() override;

	void SetSocketPos(FVector InSocketLocation) { SocketLocation = InSocketLocation; }
protected:

	FVector SocketLocation;
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;
};