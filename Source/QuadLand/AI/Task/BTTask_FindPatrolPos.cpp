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

	//NavigationMeshVolume의 값을 가져온다.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); //컨트롤하는 Pawn이 위치한 월드를 가져옴

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
	
	if (PrePosition == FVector(0.f, 0.f, 0.f))
	{
		PrePosition = Origin;
		UE_LOG(LogTemp, Log, TEXT("Init"));
	}

	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(PrePosition, 500.0f ,NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPosition"), NextPatrolPos.Location);
		
		PrePosition = NextPatrolPos.Location;
		return Type;
	}

	return EBTNodeResult::Failed;
}
