// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGC_DamageEffect.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
UQLGC_DamageEffect::UQLGC_DamageEffect()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMaterialRef(TEXT("/Script/Engine.Material'/Game/QuadLand/Materials/blood_Mat.blood_Mat'"));

	if (DecalMaterialRef.Object)
	{
		BloodDecal = DecalMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BulletMaterialRef(TEXT("/Script/Engine.Material'/Game/QuadLand/Materials/bullethole_Mat.bullethole_Mat'"));

	if (BulletMaterialRef.Object)
	{
		BulletMarks = BulletMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Default.P_Default'"));
	if (ExplosionRef.Object)
	{
		ParticleSystem = ExplosionRef.Object;
	}
}

bool UQLGC_DamageEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();

	//구조 변경
	//해당 액터의 TArray<TObjectPtr<UDecalComponent>> 가지고 있는 놈 하나 만들고 부착할 때마다 추가 예정 
	if (HitResult!=nullptr)
	{
		const IQLLifestoneContainerInterface* TargetActor = Cast<IQLLifestoneContainerInterface>(HitResult->GetActor());
		const ACharacter* Character = Cast<ACharacter>(HitResult->GetActor());
		UDecalComponent* SpawnedDecal = nullptr;
		if (TargetActor)
		{
			//이놈은 멀티캐스트가 안되기 때문에, 서버에서도 생성 클라에서도 생성해야함. 멀티캐스트 사용해서 수행해야할듯
			SpawnedDecal = UGameplayStatics::SpawnDecalAttached(BloodDecal, FVector(20.0f, 20.0f, 20.0f), Character->GetMesh(), FName("TargetActorBloodDecals"), HitResult->ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, -1.0f);
			if (SpawnedDecal)
			{
				SpawnedDecal->SortOrder = 2;
				SpawnedDecal->AttachToComponent(HitResult->GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
		
		UDecalComponent* SpawnedBulletDecal = UGameplayStatics::SpawnDecalAttached(BulletMarks, FVector(5.0f, 5.0f, 5.0f), HitResult->GetComponent(), FName("TargetActorBulletDecals"), HitResult->ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
		
		if (SpawnedBulletDecal)
		{
			SpawnedBulletDecal->SortOrder = 1;
			SpawnedBulletDecal->AttachToComponent(HitResult->GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}

		UGameplayStatics::SpawnEmitterAtLocation(HitResult->GetActor(), ParticleSystem, HitResult->Location, FRotator::ZeroRotator, true);

	}
	//총자국 Decal Spawn 

	return false;
}
