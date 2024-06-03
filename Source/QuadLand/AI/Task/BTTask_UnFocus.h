// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UnFocus.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTTask_UnFocus : public UBTTaskNode
{
	GENERATED_BODY()
	

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
