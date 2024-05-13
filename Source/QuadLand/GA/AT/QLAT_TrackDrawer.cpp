// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_TrackDrawer.h"
#include "Character/QLCharacterPlayer.h"
#include "Item/QLBomb.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"

UQLAT_TrackDrawer::UQLAT_TrackDrawer()
{
	bTickingTask = true;
}

void UQLAT_TrackDrawer::TickTask(float DeltaTime)
{
	
	if (Character)
	{
		//발사지점
//Bomb를 가져온다
		FVector CurrentForward = Character->GetActorForwardVector();
		FVector DeltaForward = PreForward - CurrentForward;

		if (DeltaForward.Length() >= 0.5f)
		{
			FVector CameraForward = Character->GetCameraForward();
			FVector StartLoc = Character->GetMesh()->GetSocketLocation(FName("Bomb"));
			//FVector TargetLoc = 
			FVector LaunchVelocity = CameraForward *1200.0f; //잠깐 박아놓기

			FPredictProjectilePathParams PredictParams(10.0f, StartLoc, LaunchVelocity, 2.0f);
			PredictParams.DrawDebugTime = 3.0f;
			PredictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
			PredictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
			FPredictProjectilePathResult Result;

			UGameplayStatics::PredictProjectilePath(this, PredictParams, Result);
			PreForward = CurrentForward;
		}

	}
}

void UQLAT_TrackDrawer::Activate()
{
	Super::Activate();
	
	Character = Cast<AQLCharacterPlayer>(GetAvatarActor());
	if (Character)
	{
		PreForward = Character->GetActorForwardVector();
	}
}


UQLAT_TrackDrawer* UQLAT_TrackDrawer::CreateTask(UGameplayAbility* OwningAbility, bool bTestAlreadyReleased)
{
	UQLAT_TrackDrawer* Task = NewAbilityTask<UQLAT_TrackDrawer>(OwningAbility);
	Task->bTestInitialState = bTestAlreadyReleased;
	return Task;
}
