// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_SelectTarget.h"
#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"

EBTNodeResult::Type UBTTask_SelectTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	AQLCharacterNonPlayer* NonPlayer = CastChecked<AQLCharacterNonPlayer>(ControllingPawn);

	if (NonPlayer->GetSelectTarget())
	{
		NonPlayer->CanSelectTarget(false);
	}
	else
	{
		NonPlayer->CanSelectTarget(true);
	}

	return EBTNodeResult::Succeeded;
}
