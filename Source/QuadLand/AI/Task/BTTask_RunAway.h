// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RunAway.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTTask_RunAway : public UBTTaskNode
{
	GENERATED_BODY()
protected:

	UBTTask_RunAway();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	bool IsLocationFreeOfObjects(const APawn* ThisCharacter,const FVector& Location);

	int32 MaxAttempts;
};
