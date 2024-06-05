// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FowardWalk.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_FowardWalk::UBTService_FowardWalk()
{
	NodeName = TEXT("FowardWalk");
	Interval = 1.0f; //1초에 한번씩 TickNode 실행
}

void UBTService_FowardWalk::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();

	ACharacter* ControllingPawn = Cast<ACharacter>(AIController->GetPawn());
	
	if (ControllingPawn == nullptr)
	{
		return;
	}

	APawn *TargetActor =Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	
	if (TargetActor)
	{
		FVector Location = ControllingPawn->GetActorLocation() + ControllingPawn->GetActorForwardVector() * 100.0f;
		AIController->MoveToLocation(Location);
	}
	//ControllingPawn->AddMovementInput(ControllingPawn->GetActorForwardVector(), 1.f);
}
