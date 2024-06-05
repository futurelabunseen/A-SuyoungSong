// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_RunAway.h"
#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Physics/QLCollision.h"

UBTTask_RunAway::UBTTask_RunAway()
{
	MaxAttempts = 5;
}

EBTNodeResult::Type UBTTask_RunAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	FNavLocation RunAwayPoint;

	for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
	{
		if (NavSystem->GetRandomPointInNavigableRadius(ControllingPawn->GetActorLocation(), 1000.0f, RunAwayPoint))
		{
			if (IsLocationFreeOfObjects(ControllingPawn, RunAwayPoint.Location))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPosition"), RunAwayPoint.Location);
				return Type;
			}
		}
		//도달 가능한 RandomPoint를 찾는다.
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPosition"), RunAwayPoint.Location); //아닐경우 마지막 위치로 그냥 이동
	return Type;
}

bool UBTTask_RunAway::IsLocationFreeOfObjects(const APawn *ThisCharacter, const FVector& Location)
{
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ThisCharacter);


	bool bHit = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		Location,
		Location,
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(500.0f),
		QueryParams
	);

	return !bHit;
}
