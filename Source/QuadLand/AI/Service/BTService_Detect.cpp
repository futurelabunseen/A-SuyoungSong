// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_Detect.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Physics/QLCollision.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f; //1초에 한번씩 TickNode 실행
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return;
	}

	FVector Center = ControllingPawn->GetActorLocation(); //현재 위치에서 다른 Pawn 찾기
	UWorld* World = ControllingPawn->GetWorld();

	if (World == nullptr)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(Detect), false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(800.0f),
		CollisionQueryParams
	);

	if (bResult)
	{

		for (const auto& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());

			if (Pawn)
			{
				FVector DeltaVector = UKismetMathLibrary::GetDirectionUnitVector(ControllingPawn->GetActorLocation(), Pawn->GetActorLocation());
				float JugmentVal=FVector::DotProduct(ControllingPawn->GetActorForwardVector(), DeltaVector);
				
				UE_LOG(LogTemp, Log, TEXT("%lf this? BTService_Detect"), JugmentVal);
				if (JugmentVal >= 0)
				{
					//앞을 의미한다.
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Pawn);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, 1.f);
					return;
				}
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
}
