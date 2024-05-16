// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_FireWall.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UQLGC_FireWall::UQLGC_FireWall()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionRef(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Smoke.P_Explosion_Smoke'"));
	if (ExplosionRef.Object)
	{
		ParticleSystem = ExplosionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundRef(TEXT("/Script/Engine.SoundCue'/Game/MilitaryWeapSilver/Sound/RocketLauncher/Cues/RocketLauncher_Explosion_Cue.RocketLauncher_Explosion_Cue'"));
	if (SoundRef.Object)
	{
		FirewallSound = SoundRef.Object;
	}

}

bool UQLGC_FireWall::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{

	UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, Parameters.Location, FRotator::ZeroRotator, true);
	
	UGameplayStatics::SpawnSoundAtLocation(Target, FirewallSound, Parameters.Location, FRotator::ZeroRotator);

	return false;
}
