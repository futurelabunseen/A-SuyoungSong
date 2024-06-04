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

	//���� ����
	//�ش� ������ TArray<TObjectPtr<UDecalComponent>> ������ �ִ� �� �ϳ� ����� ������ ������ �߰� ���� 
	if (HitResult!=nullptr)
	{
		const IQLLifestoneContainerInterface* TargetActor = Cast<IQLLifestoneContainerInterface>(HitResult->GetActor());
		const ACharacter* Character = Cast<ACharacter>(HitResult->GetActor());
		UDecalComponent* SpawnedDecal = nullptr;
		if (TargetActor)
		{
			//�̳��� ��Ƽĳ��Ʈ�� �ȵǱ� ������, ���������� ���� Ŭ�󿡼��� �����ؾ���. ��Ƽĳ��Ʈ ����ؼ� �����ؾ��ҵ�
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
	//���ڱ� Decal Spawn 

	return false;
}
