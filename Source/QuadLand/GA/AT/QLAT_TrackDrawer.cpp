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
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialRef(TEXT("/Script/Engine.Material'/Game/QuadLand/Blueprints/NoCollision/M_SplineMaterial0.M_SplineMaterial0'"));

	if (MaterialRef.Object)
	{
		Material = MaterialRef.Object;
	}
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
		float Power = 1200.0f;
		FName SocketName = FName("Bomb");
		if (Character->GetThrowBomb())
		{
			GetWorld()->GetTimerManager().ClearTimer(DrawerTimerHandle);
			DrawerTimerHandle.Invalidate();
			Character->SetThrowBomb(false);
			return;
		}

		FVector CameraForward = Character->GetCameraForward(); //캐릭터의 카메라 위치를 가져온다.
		FVector StartLoc = Character->GetMesh()->GetSocketLocation(SocketName); //폭탄이 부착된 위치를 가져온다.
		FVector LaunchVelocity = CameraForward * Power;  //카메라 방향으로 던진다.

		FPredictProjectilePathParams PredictParams(10.0f, StartLoc, LaunchVelocity, 2.0f); //위치를 예측한다.
		PredictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult Result;
		UGameplayStatics::PredictProjectilePath(this, PredictParams, Result); 
		//예측된 결과값이 Result에 담기고, Result.PathData를 사용해서 SplineComponent Mesh를 업데이트한다.


		for (int i = 0; i < Result.PathData.Num(); i++)
		{
			BombPath->AddSplinePointAtIndex(Result.PathData[i].Location, i, ESplineCoordinateSpace::World);
		}

		for (int i = 0; i < Result.PathData.Num() - 2; i++)
		{

			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::Z);
			SplineMeshComponent->SetStaticMesh(StaticMesh);
			SplineMeshComponent->SetMaterial(0, Material);
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
}


UQLAT_TrackDrawer* UQLAT_TrackDrawer::CreateTask(UGameplayAbility* OwningAbility, UStaticMesh * InDrawer, bool bTestAlreadyReleased)
{
	UQLAT_TrackDrawer* Task = NewAbilityTask<UQLAT_TrackDrawer>(OwningAbility);
	Task->StaticMesh = InDrawer;
	Task->bTestInitialState = bTestAlreadyReleased;
	return Task;
}
