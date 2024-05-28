// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerLifeStone.h"
#include "Components/SphereComponent.h"
#include "Physics/QLCollision.h"
#include "GameData/QLItemData.h"

// Sets default values
AQLPlayerLifeStone::AQLPlayerLifeStone()
{
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stone"));	

	Trigger->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	Mesh->SetWorldScale3D(FVector(0.2f, 0.2f, 0.5f));
	Mesh->SetWorldRotation(FRotator(-90.0f, 0.f, 0.f));

	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);
	Mesh->SetupAttachment(Trigger);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MeterialMeshRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/GemMaterial/Material/MI_Master_12.MI_Master_12'"));

	if (MeterialMeshRef.Object)
	{

		Mesh->SetMaterial(0, MeterialMeshRef.Object);

	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticlMeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	
	if (StaticlMeshRef.Object)
	{
		Mesh->SetStaticMesh(StaticlMeshRef.Object);
	}

	RootComponent = Trigger;

	static ConstructorHelpers::FObjectFinder<UQLItemData> ItemTypeRef(TEXT("/Script/QuadLand.QLItemData'/Game/QuadLand/GameData/Stone/QLDA_LifeStone.QLDA_LifeStone'"));

	if (ItemTypeRef.Object)
	{
		Stat = ItemTypeRef.Object;
	}

	OnActorHit.AddDynamic(this, &AQLPlayerLifeStone::OnActorOverlap);
	bReplicates = true;
	SetReplicateMovement(true);

	NetCullDistanceSquared = 4000000.0f;
}


void AQLPlayerLifeStone::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		Mesh->SetSimulatePhysics(false);
	}
}