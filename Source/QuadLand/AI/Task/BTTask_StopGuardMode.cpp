// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_StopGuardMode.h"
#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"

EBTNodeResult::Type UBTTask_StopGuardMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ControllingPawn->StopDamage();

	return Type;
}
