// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_TrackDrawer.h"
#include "Character/QLCharacterPlayer.h"
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

		//if (DeltaForward.Length() >= 0.3f)
		{
			FVector CameraForward = Character->GetCameraForward();
			FVector StartLoc = Character->GetMesh()->GetSocketLocation(FName("Bomb")); 
			FVector LaunchVelocity = CameraForward *1200.0f; //잠깐 박아놓기

			FPredictProjectilePathParams PredictParams(10.0f, StartLoc, LaunchVelocity, 2.0f);
			PredictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
			FPredictProjectilePathResult Result;
			UGameplayStatics::PredictProjectilePath(this, PredictParams, Result);
			
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


UQLAT_TrackDrawer* UQLAT_TrackDrawer::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class AActor> InDrawer, bool bTestAlreadyReleased)
{
	UQLAT_TrackDrawer* Task = NewAbilityTask<UQLAT_TrackDrawer>(OwningAbility);
	Task->Drawer = InDrawer;
	Task->bTestInitialState = bTestAlreadyReleased;
	return Task;
}
