// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_TakeDamage.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"

UBTDecorator_TakeDamage::UBTDecorator_TakeDamage()
{
	NodeName = TEXT("TakeDamage");
}

bool UBTDecorator_TakeDamage::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	return ControllingPawn->CanTakeDamage();
}
