// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_ImpactByGun.h"
#include "Character/QLCharacterPlayer.h"
#include "Character/QLCharacterBase.h"
#include "Item/QLWeaponComponent.h"
#include "Physics/QLCollision.h"
#include "Kismet/GameplayStatics.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item/QLBullet.h"
#include "Item/QLObjectPooling.h"

UQLGC_ImpactByGun::UQLGC_ImpactByGun()
{
}

bool UQLGC_ImpactByGun::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(Parameters.Instigator);

	if (Character)
	{
		//시작점 Muzzle 입구
		const FVector &TargetPos = Parameters.Location;
		FVector OriginalPos;

		AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);
		UQLWeaponComponent* Weapon = Character->GetWeapon();
		FVector MuzzlePos = Weapon->GetWeaponMesh()->GetSocketLocation(FName("MuzzleFlash"));
		if (Player)
		{
			OriginalPos = Player->CalPlayerLocalCameraStartPos();
		}
		else
		{
			//Muzzle위치로?
			OriginalPos = MuzzlePos;
		}

		//Projecttile 써서 목적지로 출발하는 수밖에 없나봄.. OriginalPos 생성
		//TargetPos 로 출발 

		if (Character->HasAuthority())
		{
			FVector StartLocation = MuzzlePos;

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
			FQuat Quat;

			if (bResult)
			{
				Quat = UKismetMathLibrary::FindLookAtRotation(OutHitResult.Location, OutHitResult.TraceEnd).Quaternion();
			}
			else
			{
				Quat = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetPos).Quaternion();
			}

			BulletTransform.SetRotation(Quat);

			UQLObjectPooling *ObjectPooling = Weapon->GetObjectPoolingManager();
			if (ObjectPooling)
			{
				AQLBullet * Bullet = Cast<AQLBullet>(ObjectPooling->GetObject());
				
				if (Bullet == nullptr)
				{
					Bullet = GetWorld()->SpawnActor<AQLBullet>(BulletClass, BulletTransform);
					Bullet->SetOwner(Player);
				}
				else
				{
					Bullet->SetActorLocation(StartLocation);
					Bullet->SetActorTransform(BulletTransform);
				}

			}
			//사용하기 전에 있으면 가져온다. 
		}
	}

	//끝점 Target 위치 
	return false;
}
