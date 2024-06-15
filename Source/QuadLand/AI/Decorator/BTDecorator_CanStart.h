// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanStart.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTDecorator_CanStart : public UBTDecorator
{
	GENERATED_BODY()
	

public:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

};
