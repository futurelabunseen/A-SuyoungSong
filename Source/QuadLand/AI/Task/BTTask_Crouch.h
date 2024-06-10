// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Crouch.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UBTTask_Crouch : public UBTTaskNode
{
	GENERATED_BODY()
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:

	void UnCrouch(APawn* ControllingPawn);
};
