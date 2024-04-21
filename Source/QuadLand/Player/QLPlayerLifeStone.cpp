// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerLifeStone.h"
#include "Components/SphereComponent.h"
#include "Physics/QLCollision.h"

// Sets default values
AQLPlayerLifeStone::AQLPlayerLifeStone()
{
	ASC = nullptr;
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stone"));	

	Trigger->SetWorldScale3D(FVector(1.f, 1.f, 2.f));
	Mesh->SetWorldScale3D(FVector(1.f, 1.f, 2.f));
	Mesh->SetWorldRotation(FRotator(-90.0f, 0.f, 0.f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
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
}

UAbilitySystemComponent* AQLPlayerLifeStone::GetAbilitySystemComponent() const
{
	return ASC;
}

