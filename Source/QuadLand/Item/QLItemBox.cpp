// Fill out your copyright notice in the Description page of Project Settings.


#include "QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameData/QLWeaponStat.h"
#include "Item/QLWeaponItemBox.h"
#include "Physics/QLCollision.h"
#include "QuadLand.h"

// Sets default values
AQLItemBox::AQLItemBox()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));
	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f), FRotator(90.f, -90.f, 0.f)); //Mesh 기준이기 때문에 아이템에서는 변경 예정
	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);

	OnActorHit.AddDynamic(this, &AQLItemBox::OnActorOverlap);
	bReplicates = true;
	SetReplicateMovement(true);

	NetCullDistanceSquared = 4000000.0f;
}

void AQLItemBox::InitPosition(const FVector& Location)
{
	if (HasAuthority())
	{
		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //식별자 

		FHitResult OutHitResult;

		FVector StartLocation = Location;

		FVector EndLocation = StartLocation + 150.0f * GetActorUpVector() * -1;
		bool bResult = GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			StartLocation,
			EndLocation,
			CCHANNEL_QLGROUND,
			CollisionParams
		);

		if (bResult)
		{
			OutHitResult.Location.Z += GetZPos();
			SetActorLocation(OutHitResult.Location);
		}

		AQLWeaponItemBox* WeaponBox = Cast<AQLWeaponItemBox>(this);

		if (WeaponBox)
		{
			WeaponBox->SpawnBulletsAround();
		}

	}
}

float AQLItemBox::GetZPos()
{
	return Trigger->GetScaledBoxExtent().Z / 2.0f;
}

void AQLItemBox::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		Mesh->SetSimulatePhysics(false);
	}
}
