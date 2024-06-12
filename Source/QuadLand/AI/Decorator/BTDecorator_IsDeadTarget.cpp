// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_IsDeadTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
#include "Game/QLGameMode.h"

UBTDecorator_IsDeadTarget::UBTDecorator_IsDeadTarget()
{
}

bool UBTDecorator_IsDeadTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

	AQLGameMode *GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode == nullptr)
	{
		return false;
	}


	return GameMode->GetPlayerState(FName(ControllingPawn->GetName()));
}
