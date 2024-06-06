// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_Dead.h"
#include "AI/QLAIController.h"
#include "Character/QLCharacterNonPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"

EBTNodeResult::Type UBTTask_Dead::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FGameplayTagContainer TagContainer(CHARACTER_STATE_DEAD);
	ASC->TryActivateAbilitiesByTag(TagContainer);

	return Type;
}
