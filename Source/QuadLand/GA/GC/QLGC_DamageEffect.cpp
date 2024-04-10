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

	//���� ����
	//�ش� ������ TArray<TObjectPtr<UDecalComponent>> ������ �ִ� �� �ϳ� ����� ������ ������ �߰� ���� 
	if (HitResult!=nullptr)
	{
		const IQLReceivedDamageInterface* TargetActor = Cast<IQLReceivedDamageInterface>(HitResult->GetActor());
		const ACharacter* Character = Cast<ACharacter>(HitResult->GetActor());
		UDecalComponent* SpawnedDecal = nullptr;
		if (TargetActor )
		{
			UE_LOG(LogTemp, Log, TEXT("find a player"));

			//�̳��� ��Ƽĳ��Ʈ�� �ȵǱ� ������, ���������� ���� Ŭ�󿡼��� �����ؾ���. ��Ƽĳ��Ʈ ����ؼ� �����ؾ��ҵ�
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
	//���ڱ� Decal Spawn 

	return false;
}
