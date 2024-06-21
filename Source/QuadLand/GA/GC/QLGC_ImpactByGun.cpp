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
		//������ Muzzle �Ա�
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
			//Muzzle��ġ��?
			OriginalPos = MuzzlePos;
		}

		//Projecttile �Ἥ �������� ����ϴ� ���ۿ� ������.. OriginalPos ����
		//TargetPos �� ��� 

		if (Character->HasAuthority())
		{
			FVector StartLocation = MuzzlePos;

			FTransform BulletTransform;
			BulletTransform.SetLocation(StartLocation);
			FCollisionQueryParams Params(SCENE_QUERY_STAT(LineTraceResult), false, Character); //�ĺ��� 

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
			//����ϱ� ���� ������ �����´�. 
		}
	}

	//���� Target ��ġ 
	return false;
}
