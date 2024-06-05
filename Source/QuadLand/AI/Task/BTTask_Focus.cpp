// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_Focus.h"
#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
EBTNodeResult::Type UBTTask_Focus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (Target)
	{
		AIController->SetFocus(Target);
		//Rotation Yaw => Target쪽으로 돌린다..?
		//NonPlayer->SetAiming(true);
		//Aim True로
	}
	else
	{
		AIController->SetFocalPoint(GetRandomPointInViewCone(ControllingPawn));
		//NonPlayer->SetAiming(false);
	}
	
	return Type;
}

FVector UBTTask_Focus::GetRandomPointInViewCone(const APawn *ControllingPawn)
{
	
	if (ControllingPawn == nullptr)
	{
		return FVector(0.f,0.f,0.f);

	}

	FVector ForwardVector = ControllingPawn->GetActorForwardVector();

	float RandomYaw = FMath::RandRange(-90.f, 90.f);

	FRotator Rotation(0.f, RandomYaw, 0.f);

	FVector RotatedVector = Rotation.RotateVector(ForwardVector);

	FVector RandomPoint = ControllingPawn->GetActorLocation() + RotatedVector * 100.0f;

	return RandomPoint;
}
