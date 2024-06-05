// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SelectAttackOrRun.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTDecorator_SelectAttackOrRun : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_SelectAttackOrRun();

	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

};
