// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLBullet.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
// Sets default values
AQLBullet::AQLBullet()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Tracer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Tracer"));
	Tracer->SetupAttachment(Box);

	static  ConstructorHelpers::FObjectFinder<UParticleSystem>  ParticleAssetRef(TEXT("/Script/Engine.ParticleSystem'/Game/MilitaryWeapSilver/FX/P_AssaultRifle_Tracer_01.P_AssaultRifle_Tracer_01'"));
	if (ParticleAssetRef.Object)
	{ 
		Tracer->SetTemplate(ParticleAssetRef.Object);
	}

	ProjecttileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjecttileMovement"));

	RootComponent = Box;

	bReplicates = true;
}

void AQLBullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.0f);
}
