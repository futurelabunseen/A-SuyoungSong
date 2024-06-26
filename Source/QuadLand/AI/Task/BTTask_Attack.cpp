// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_Attack.h"
#include "AI/QLAIController.h"
#include "Character/QLCharacterNonPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_PlayerStat.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	//NavigationMeshVolume의 값을 가져온다.
	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = ControllingPawn->GetAbilitySystemComponent();

	if (ASC == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
	{
		return EBTNodeResult::Failed;
	}

	const UQLAS_PlayerStat *Stat = ASC->GetSet<UQLAS_PlayerStat>();

	if (Stat->GetHealth() <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayTagContainer TagContainer(ControllingPawn->GetCurrentAttackTag());
	ASC->TryActivateAbilitiesByTag(TagContainer);
	return Type;
}
