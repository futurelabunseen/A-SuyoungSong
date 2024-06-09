// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_TurnToTarget.h"
#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	//NavigationMeshVolume의 값을 가져온다.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));

	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	float TurnSpeed = 30.0f;

	FVector LookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0;

	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	return Type;
}
