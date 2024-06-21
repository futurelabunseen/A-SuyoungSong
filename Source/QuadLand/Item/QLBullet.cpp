// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLBullet.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Item/QLWeaponComponent.h"
#include "Character/QLCharacterBase.h"
#include "Item/QLObjectPooling.h"
#include "QuadLand.h"

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

	Box->OnComponentBeginOverlap.AddDynamic(this, &AQLBullet::OnOverlapBegin);
}

void AQLBullet::Init()
{
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetOwner());
	if (Character)
	{
		UQLWeaponComponent* Weapon = Character->GetWeapon();
		if (Weapon)
		{
			UQLObjectPooling* ObjectPool = Weapon->GetObjectPoolingManager();

			if (ObjectPool)
			{
				QL_LOG(QLLog, Log, TEXT("Init µé¾î°¡?"));
				ObjectPool->PutBackToPool(this);
			}
		}
	}
}

void AQLBullet::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AQLBullet::Init, 3.0f, false);
}

void AQLBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (ResetTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ResetTimer);
		ResetTimer.Invalidate();
	}
	Init();
}

