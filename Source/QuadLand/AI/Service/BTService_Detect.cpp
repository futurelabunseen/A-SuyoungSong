// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_Detect.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Physics/QLCollision.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f; //1�ʿ� �ѹ��� TickNode ����
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return;
	}

	FVector Center = ControllingPawn->GetActorLocation(); //���� ��ġ���� �ٸ� Pawn ã��
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

	bool bReset = false;
	//Pawn ����
	if (bResult)
	{

		for (const auto& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			
			if (Pawn)
			{
				FVector TargetVector = Pawn->GetActorLocation();
				TargetVector.Z = 0.f;

				FVector OriginVector = ControllingPawn->GetActorLocation();
				OriginVector.Z = 0.f;

				FVector DeltaVector = UKismetMathLibrary::GetDirectionUnitVector(OriginVector, TargetVector);
				float JugmentVal=FVector::DotProduct(ControllingPawn->GetActorForwardVector(), DeltaVector);
				
				if (JugmentVal >= 0)
				{
					UE_LOG(LogTemp, Log, TEXT("%s %s = %s"), *TargetVector.ToString(), *OriginVector.ToString(), *DeltaVector.ToString());
					//���� �ǹ��Ѵ�.
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Pawn);
					return;
				}
			}

			float Dist = FVector::Dist(Pawn->GetActorLocation(), ControllingPawn->GetActorLocation());
			
			bReset = bReset || Dist <= 200.f;
		}

		//Pawn���� �Ÿ��� 200.�̻��̸� nullptr����
		if (bReset)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
		}

	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
	}

}
