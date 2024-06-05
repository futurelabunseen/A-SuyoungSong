// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FowardWalk.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTService_FowardWalk : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FowardWalk();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
