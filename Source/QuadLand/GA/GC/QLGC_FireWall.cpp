// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_FireWall.h"

UQLGC_FireWall::UQLGC_FireWall()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Fire/P_Fire_Wall.P_Fire_Wall'"));
	if (ExplosionRef.Object)
	{
		ParticleSystem = ExplosionRef.Object;
	}

}

bool UQLGC_FireWall::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{

//	UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, TargetActor.Get()->GetActorLocation(), FRotator::ZeroRotator, true);

	return false;
}
