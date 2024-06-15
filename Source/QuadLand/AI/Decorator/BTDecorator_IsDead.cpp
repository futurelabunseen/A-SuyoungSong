// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_IsDead.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"

UBTDecorator_IsDead::UBTDecorator_IsDead()
{
	NodeName = TEXT("IsDead");
}


bool UBTDecorator_IsDead::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bResult == false)
	{
		return bResult;
	}

	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = ControllingPawn->GetAbilitySystemComponent();

	if (ASC == nullptr)
	{
		return false;
	}

	FGameplayTagContainer TagContainer(CHARACTER_STATE_DEAD);
	return ASC->HasAnyMatchingGameplayTags(TagContainer);
}
