// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_TrackDrawer.h"
#include "Character/QLCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h" //SplineComponent ->Mesh를 채워줘야한다.
#include "Engine/StaticMesh.h"
#include "Engine/EngineTypes.h"

#include "QuadLand.h"
UQLAT_TrackDrawer::UQLAT_TrackDrawer()
{
}


void UQLAT_TrackDrawer::Activate()
{
	Super::Activate();
	
	Character = Cast<AQLCharacterPlayer>(GetAvatarActor());
	if (Character)
	{
		BombPath = Character->GetBombPath();
		if (BombPathMeshComp.Num() > 0)
		{
			BombPath->ClearSplinePoints(true);
			for (int32 Index = 0; Index < BombPathMeshComp.Num(); Index++)
			{
				if (BombPathMeshComp[Index])
				{
					BombPathMeshComp[Index]->DestroyComponent();
				}
			}
			BombPathMeshComp.Empty();
		}
	}

	GetWorld()->GetTimerManager().SetTimer(DrawerTimerHandle, this, &UQLAT_TrackDrawer::Recursive, 0.01f, true,0.5f);
}

void UQLAT_TrackDrawer::OnDestroy(bool bInOwnerFinished)
{
	if (BombPathMeshComp.Num() > 0)
	{
		BombPath->ClearSplinePoints(true);
		for (int32 Index = 0; Index < BombPathMeshComp.Num(); Index++)
		{
			if (BombPathMeshComp[Index])
			{
				BombPathMeshComp[Index]->DestroyComponent();
			}
		}
		BombPathMeshComp.Empty();
	}
	BombPath = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("Current? Bomb"));
}


UQLAT_TrackDrawer* UQLAT_TrackDrawer::CreateTask(UGameplayAbility* OwningAbility, UStaticMesh * InDrawer, bool bTestAlreadyReleased)
{
	UQLAT_TrackDrawer* Task = NewAbilityTask<UQLAT_TrackDrawer>(OwningAbility);
	Task->StaticMesh = InDrawer;
	Task->bTestInitialState = bTestAlreadyReleased;
	return Task;
}

void UQLAT_TrackDrawer::Recursive()
{

	if (Character && BombPath)
	{

		if (BombPathMeshComp.Num() > 0)
		{
			BombPath->ClearSplinePoints(true);
			for (int32 Index = 0; Index < BombPathMeshComp.Num(); Index++)
			{
				if (BombPathMeshComp[Index])
				{
					BombPathMeshComp[Index]->DestroyComponent();
				}
			}
			BombPathMeshComp.Empty();
		}

		if (Character->GetThrowBomb())
		{
			GetWorld()->GetTimerManager().ClearTimer(DrawerTimerHandle);
			DrawerTimerHandle.Invalidate();
			Character->SetThrowBomb(false);
			return;
		}
		FVector CameraForward = Character->GetCameraForward();
		FVector StartLoc = Character->GetMesh()->GetSocketLocation(FName("Bomb"));
		FVector LaunchVelocity = CameraForward * 1200.0f; //잠깐 박아놓기

		FPredictProjectilePathParams PredictParams(10.0f, StartLoc, LaunchVelocity, 2.0f);
		PredictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult Result;
		UGameplayStatics::PredictProjectilePath(this, PredictParams, Result);


		for (int i = 0; i < Result.PathData.Num(); i++)
		{
			BombPath->AddSplinePointAtIndex(Result.PathData[i].Location, i, ESplineCoordinateSpace::World);
		}

		for (int i = 0; i < Result.PathData.Num() - 2; i++)
		{

			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::Z);
			SplineMeshComponent->SetStaticMesh(StaticMesh);
			SplineMeshComponent->SetMobility(EComponentMobility::Stationary);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;

			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent->AttachToComponent(BombPath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent->SetStartScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));
			SplineMeshComponent->SetEndScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));

			FVector StartPoint = BombPath->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
			FVector StartTangent = BombPath->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
			FVector EndPoint = BombPath->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
			FVector EndTangent = BombPath->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

			SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			BombPathMeshComp.Add(SplineMeshComponent);

		}
		
	}
}
