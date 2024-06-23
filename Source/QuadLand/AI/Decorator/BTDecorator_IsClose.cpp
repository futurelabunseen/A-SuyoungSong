// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_IsClose.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Physics/QLCollision.h"
UBTDecorator_IsClose::UBTDecorator_IsClose()
{
	NodeName = TEXT("IsClose");

}

bool UBTDecorator_IsClose::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(Detect), false, ControllingPawn);

	bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		ControllingPawn->GetActorLocation(),
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(500.0f),
		CollisionQueryParams
	);

	return bResult;
}
