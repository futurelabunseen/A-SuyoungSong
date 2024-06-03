// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AQLAIController::AQLAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardDataRef(TEXT("/Script/AIModule.BlackboardData'/Game/QuadLand/AI/BB_QLNonPlayer.BB_QLNonPlayer'"));

	if (BlackboardDataRef.Object)
	{
		BBAsset = BlackboardDataRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(TEXT("/Script/AIModule.BehaviorTree'/Game/QuadLand/AI/BT_QLNonPlayer.BT_QLNonPlayer'"));

	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}

}

void AQLAIController::RunAI()
{
	//Backboard�κ��� BlackboardComponent�� �����;��Ѵ�.
	UBlackboardComponent *BCPtr = Blackboard.Get();

	if (UseBlackboard(BBAsset, BCPtr)) //BBAsset�� ����� ������ Blackboard ����
	{ 
		Blackboard->SetValueAsVector(TEXT("HomePosition"), GetPawn()->GetActorLocation());
		bool RunResult = RunBehaviorTree(BTAsset); //���� Blackboard�� BehaviorTree�� �����ϵ��� ��.
		ensure(RunResult);
	}
}

void AQLAIController::StopAI()
{
	//BehaviorTree�� ��� BrainComponent�� ����
	UBehaviorTreeComponent* BTCPtr = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTCPtr)
	{
		BTCPtr->StopTree();
	}
}

void AQLAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();

}
