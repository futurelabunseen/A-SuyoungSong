// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AQLAIController::AQLAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardDataRef(TEXT("/Script/AIModule.BlackboardData'/Game/QuadLand/AI/BB_EQSTest.BB_EQSTest'"));

	if (BlackboardDataRef.Object)
	{
		BBAsset = BlackboardDataRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(TEXT("/Script/AIModule.BehaviorTree'/Game/QuadLand/AI/BT_EQSTest.BT_EQSTest'"));

	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}

}

void AQLAIController::RunAI()
{
	//Backboard로부터 BlackboardComponent를 가져와야한다.
	UBlackboardComponent *BCPtr = Blackboard.Get();

	if (UseBlackboard(BBAsset, BCPtr)) //BBAsset을 사용할 것으로 Blackboard 지정
	{ 
		Blackboard->SetValueAsVector(TEXT("HomePosition"), GetPawn()->GetActorLocation());
		bool RunResult = RunBehaviorTree(BTAsset); //현재 Blackboard를 BehaviorTree가 실행하도록 함.
		ensure(RunResult);
	}
}

void AQLAIController::StopAI()
{
	//BehaviorTree의 경우 BrainComponent에 저장
	UBehaviorTreeComponent* BTCPtr = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTCPtr)
	{
		BTCPtr->StopTree();
	}
}

const APawn* AQLAIController::GetTarget()
{
	UBlackboardComponent* BC = Blackboard.Get();
	if (BC)
	{
		return Cast<APawn>(BC->GetValueAsObject(TEXT("TargetActor")));
	}

	return nullptr;
}

FVector AQLAIController::GetTargetPos()
{
	return FVector();
}

void AQLAIController::SetTargetPos(FVector InPos)
{
	TargetPos = GetTarget()->GetActorLocation() + InPos;
}

void AQLAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();

}
