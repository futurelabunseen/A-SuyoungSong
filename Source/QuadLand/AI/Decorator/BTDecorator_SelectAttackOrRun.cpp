// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_SelectAttackOrRun.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"

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
	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return false;
	}
	UAbilitySystemComponent* ASC = ControllingPawn->GetAbilitySystemComponent();

	if (ASC == nullptr)
	{
		return EBTNodeResult::Failed;
	}


	const UQLAS_PlayerStat *NonPlayerHp = ASC->GetSet<UQLAS_PlayerStat>();

	float RemainingHP = NonPlayerHp->GetHealth() / NonPlayerHp->GetMaxHealth();

	return RemainingHP >= 0.3f; //공격할 수 있다
}
