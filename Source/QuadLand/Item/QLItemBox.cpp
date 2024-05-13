// Fill out your copyright notice in the Description page of Project Settings.


#include "QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameData/QLWeaponStat.h"
#include "Physics/QLCollision.h"
#include "QuadLand.h"

// Sets default values
AQLItemBox::AQLItemBox() : Power(100.0f), Radius(100.0f)
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
}

void AQLItemBox::InitPosition()
{
	if (HasAuthority())
	{
		FVector StartLoc = Mesh->GetRelativeLocation();

		float Theta = FMath::FRandRange(-180.f, 180.0f);

		float XValue = Radius * FMath::Cos(Theta);
		float YValue = Radius * FMath::Sin(Theta);

		FVector TargetLoc(XValue, YValue, 0.0f); //방향

		Mesh->AddImpulse(TargetLoc * Power); //방향 * 힘
	}
}

void AQLItemBox::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Trigger->SetSimulatePhysics(false);
	Mesh->SetSimulatePhysics(false);

	QL_LOG(QLLog, Warning, TEXT("current false"));
	Trigger->SetRelativeLocation(Mesh->GetRelativeLocation());
}