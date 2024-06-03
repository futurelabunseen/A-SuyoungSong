// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_FindPatrolPos.h"
#include "AI/QLAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	//NavigationMeshVolume�� ���� �����´�.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); //��Ʈ���ϴ� Pawn�� ��ġ�� ���带 ������

	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("HomePosition"));
	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 1000.0f ,NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPosition"), NextPatrolPos.Location);
		return Type;
	}

	return EBTNodeResult::Failed;
}
