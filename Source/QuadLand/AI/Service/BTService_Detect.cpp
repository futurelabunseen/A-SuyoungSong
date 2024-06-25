// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_Detect.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Physics/QLCollision.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemComponent.h"
#include "Game/QLGameMode.h"
#include "AI/QLAIController.h"
#include "Character/QLCharacterNonPlayer.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("LiveTarget");
	Interval = 1.0f; //1초에 한번씩 TickNode 실행
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AQLAIController* Controller = Cast<AQLAIController>(OwnerComp.GetAIOwner()->GetPawn());

	if (Controller == nullptr)
	{
		return;
	}

	AQLCharacterNonPlayer* ControllingPawn = Cast<AQLCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return;
	}

	AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode == nullptr)
	{
		return;
	}
	
	bool bLiveTarget = GameMode->GetPlayerState(FName(ControllingPawn->GetName()));

	if (bLiveTarget == false)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
	}

	return;
}
