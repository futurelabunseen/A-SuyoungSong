// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_Crouch.h"
#include "AI/QLAIController.h"
#include "Character/QLCharacterNonPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UBTTask_Crouch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AQLCharacterNonPlayer* NonPlayer = CastChecked<AQLCharacterNonPlayer>(ControllingPawn);

	bool RandomBool = UKismetMathLibrary::RandomBool();
	
	if (RandomBool)
	{
		if (NonPlayer->CanCrouch())
		{
			NonPlayer->Crouch();
		}
		else
		{
			NonPlayer->UnCrouch();
		}
	}
	return Type;
}
