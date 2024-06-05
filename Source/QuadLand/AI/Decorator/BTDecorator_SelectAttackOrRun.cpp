// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_SelectAttackOrRun.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_SelectAttackOrRun::UBTDecorator_SelectAttackOrRun()
{
	NodeName = TEXT("CanAttack");
}


bool UBTDecorator_SelectAttackOrRun::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bResult == false)
	{
		return bResult;
	}
	//Random으로정한다.
	float Value = FMath::RandRange(0, 100);

	UE_LOG(LogTemp, Log, TEXT("UBTDecorator_SelectAttackOrRun %d"), Value >= 50.f);
	return Value>=50.f;
}
