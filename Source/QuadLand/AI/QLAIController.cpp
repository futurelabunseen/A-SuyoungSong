// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/QLAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/QLCharacterNonPlayer.h"
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

	bCanStartForAI = false;
}

void AQLAIController::RunAI()
{
	//Backboard로부터 BlackboardComponent를 가져와야한다.
	UBlackboardComponent *BCPtr = Blackboard.Get();

	FTimerHandle WaitTimer;
	if (UseBlackboard(BBAsset, BCPtr)) //BBAsset을 사용할 것으로 Blackboard 지정
	{ 
		Blackboard->SetValueAsVector(TEXT("HomePosition"), GetPawn()->GetActorLocation());
		bool RunResult = RunBehaviorTree(BTAsset); //현재 Blackboard를 BehaviorTree가 실행하도록 함.
		ensure(RunResult);
	}

	GetWorld()->GetTimerManager().SetTimer(WaitTimer, this, &AQLAIController::StartAI, DelayAITime, false);
	//GameState를 가져온다.
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
void AQLAIController::StartAI()
{
	bCanStartForAI = true;
	UE_LOG(LogTemp, Log, TEXT("Start AI"));
}
FVector AQLAIController::GetTargetPos()
{
	return TargetLocation;
}

void AQLAIController::SetTargetPos()
{
	AQLCharacterNonPlayer* NonPlayer = CastChecked<AQLCharacterNonPlayer>(GetPawn());

	FVector2D XRandOffset = NonPlayer->GetXRandOffset(); //X
	FVector2D ZRandOffset = NonPlayer->GetZRandOffset(); //Z

	float RandX = FMath::RandRange(XRandOffset.X, XRandOffset.Y);
	float RandY = FMath::RandRange(ZRandOffset.X, ZRandOffset.Y);

	const APawn *Target = GetTarget();
	if (Target)
	{
		TargetLocation = GetTarget()->GetActorLocation();
		TargetLocation.X += RandX;
		TargetLocation.Z += RandY;
	}
}

void AQLAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();

}
