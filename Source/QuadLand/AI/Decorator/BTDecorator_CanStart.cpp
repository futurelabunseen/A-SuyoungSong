// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_CanStart.h"
#include "Game/QLGameMode.h"
#include "AI/QLAIController.h"
bool UBTDecorator_CanStart::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);


	AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode == nullptr)
	{
		return false;
	}
	AQLAIController *AIController =Cast<AQLAIController>(OwnerComp.GetAIOwner());

	if (AIController == nullptr)
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("UBTDecorator_CanStart %d"), AIController->GetCanStartForAI());
	return AIController->GetCanStartForAI();
}
