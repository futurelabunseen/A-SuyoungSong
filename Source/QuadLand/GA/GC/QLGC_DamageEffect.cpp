// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGC_DamageEffect.h"
#include "Interface/QLReceivedDamageInterface.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UQLGC_DamageEffect::UQLGC_DamageEffect()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMaterialRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Default.P_Default'"));

	if (DecalMaterialRef.Object)
	{
		BloodDecal = DecalMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BulletMaterialRef(TEXT("/Script/Engine.Material'/Game/QuadLand/Materials/bullethole_Mat.bullethole_Mat'"));

	if (BulletMaterialRef.Object)
	{
		BulletMarks = BulletMaterialRef.Object;
	}
}

bool UQLGC_DamageEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();

	//구조 변경
	//해당 액터의 TArray<TObjectPtr<UDecalComponent>> 가지고 있는 놈 하나 만들고 부착할 때마다 추가 예정 
	if (HitResult!=nullptr)
	{
		const IQLReceivedDamageInterface* TargetActor = Cast<IQLReceivedDamageInterface>(HitResult->GetActor());
		const ACharacter* Character = Cast<ACharacter>(HitResult->GetActor());
		UDecalComponent* SpawnedDecal = nullptr;
		if (TargetActor )
		{
			UE_LOG(LogTemp, Log, TEXT("find a player"));

			//이놈은 멀티캐스트가 안되기 때문에, 서버에서도 생성 클라에서도 생성해야함. 멀티캐스트 사용해서 수행해야할듯
			SpawnedDecal = UGameplayStatics::SpawnDecalAttached(BloodDecal, FVector(20.0f, 20.0f, 20.0f), Character->GetMesh(), FName("TargetActorBloodDecals"), HitResult->ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, -1.0f);
			if (SpawnedDecal)
			{
				SpawnedDecal->SortOrder = 2;
				SpawnedDecal->AttachToComponent(HitResult->GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
		
		UDecalComponent* SpawnedBulletDecal = UGameplayStatics::SpawnDecalAttached(BulletMarks, FVector(5.0f, 5.0f, 5.0f), HitResult->GetComponent(), FName("TargetActorBulletDecals"), HitResult->ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, -1.0f);
		
		if (SpawnedBulletDecal)
		{
			SpawnedBulletDecal->SortOrder = 1;
			SpawnedBulletDecal->AttachToComponent(HitResult->GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}

	}
	//총자국 Decal Spawn 

	return false;
}
