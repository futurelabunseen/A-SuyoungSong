// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_UnFocus.h"
#include "AI/QLAIController.h"

EBTNodeResult::Type UBTTask_UnFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AIController->ClearFocus(EAIFocusPriority::Default);
	
    return Type;
}
