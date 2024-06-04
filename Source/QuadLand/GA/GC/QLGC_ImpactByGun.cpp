// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_ImpactByGun.h"
#include "Character/QLCharacterPlayer.h"
#include "Item/QLWeaponComponent.h"
#include "Physics/QLCollision.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item/QLBullet.h"

UQLGC_ImpactByGun::UQLGC_ImpactByGun()
{
}

bool UQLGC_ImpactByGun::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(Parameters.Instigator);

	if (Character)
	{
		//시작점 Muzzle 입구
		const FVector &TargetPos = Parameters.Location;
		const FVector &OriginalPos = Character->CalPlayerLocalCameraStartPos();

		//Projecttile 써서 목적지로 출발하는 수밖에 없나봄.. OriginalPos 생성
		//TargetPos 로 출발 

		if (Character->HasAuthority())
		{
			FVector StartLocation = Character->GetWeapon()->GetWeaponMesh()->GetSocketLocation(FName("MuzzleFlash"));

			FTransform BulletTransform;
			BulletTransform.SetLocation(StartLocation);
			FCollisionQueryParams Params(SCENE_QUERY_STAT(LineTraceResult), false, Character); //식별자 

			FHitResult OutHitResult;

			bool bResult = GetWorld()->LineTraceSingleByChannel(
				OutHitResult,
				OriginalPos,
				TargetPos,
				CCHANNEL_QLACTION,
				Params
			);
			
			if (bResult)
			{
				FQuat Quat = UKismetMathLibrary::FindLookAtRotation(OutHitResult.Location, OutHitResult.TraceEnd).Quaternion();
				BulletTransform.SetRotation(Quat);
				GetWorld()->SpawnActor<AQLBullet>(BulletClass, BulletTransform);
			}
			else
			{
				FQuat Quat = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetPos).Quaternion();
				BulletTransform.SetRotation(Quat);
				GetWorld()->SpawnActor<AQLBullet>(BulletClass, BulletTransform);
			}
			
		}

	}

	//끝점 Target 위치 
	return false;
}
