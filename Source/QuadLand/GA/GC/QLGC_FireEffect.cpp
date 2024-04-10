// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_FireEffect.h"
#include "Particles/ParticleSystem.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLWeaponStat.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

UQLGC_FireEffect::UQLGC_FireEffect()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Default.P_Default'"));

	if (ParticleRef.Object)
	{
		FireParticleEffect = ParticleRef.Object;
	}

}

bool UQLGC_FireEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{

	const AQLCharacterPlayer* CurrentPlayer = Cast<AQLCharacterPlayer>(Parameters.GetSourceObject());

	if (CurrentPlayer)
	{

		FVector FireEffectLoc = CurrentPlayer->GetWeaponMesh()->GetSocketLocation(FName(TEXT("MuzzleFlash")));
		//이펙트를 터트릴 위치
		UGameplayStatics::SpawnEmitterAtLocation(MyTarget, FireParticleEffect, FireEffectLoc);
	}
	return false;
}