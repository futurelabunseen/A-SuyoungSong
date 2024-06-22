// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_DamageScore.h"
#include "GameFramework/Character.h"
#include "Gimmick/QLDamageWidgetActor.h"
bool UQLGC_DamageScore::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{

	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	float Damage = Parameters.RawMagnitude;

	if (Damage <= 1.0f)
	{
		return false;
	}
	if (HitResult)
	{
		FActorSpawnParameters Params;
		ACharacter* Character = Cast<ACharacter>(HitResult->GetActor());

		if (Character->HasAuthority())
		{
			AActor* DamageActor = GetWorld()->SpawnActor<AActor>(DamageWidgetClass, Parameters.Location, FRotator::ZeroRotator, Params);
			if (DamageActor)
			{
				DamageActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
				FVector Loc = HitResult->Location + FVector(-25.f, -25.f, 50.f);
				DamageActor->SetActorLocation(Loc);

				AQLDamageWidgetActor* Widget = Cast<AQLDamageWidgetActor>(DamageActor);
				if (Widget)
				{
					Widget->SetDamage(Damage);
				}
			}
		}
	}

	return false;
}
