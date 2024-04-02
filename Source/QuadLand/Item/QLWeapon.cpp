// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/QLCollision.h"

// Sets default values
AQLWeapon::AQLWeapon()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));


	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f),FRotator(90.f,0.f,0.f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

}

UStaticMeshComponent* AQLWeapon::GetMesh() const
{
	return Mesh;
}

UQLWeaponStat* AQLWeapon::GetStat() const
{
	return WeaponStat;
}

