// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Physics/QLCollision.h"
// Sets default values
AQLBomb::AQLBomb()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));

	RootComponent = Collision;
	Collision->SetCollisionProfileName(CPROFILE_QLPHYSICS);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Mesh->SetupAttachment(Collision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/QuadLand/Item3D/Bomb/Pipe_Bomb/Pipe_Bomb.Pipe_Bomb'"));
	if (MeshRef.Object)
	{
		Mesh->SetSkeletalMesh(MeshRef.Object);
	}
	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);
	//초기값은 BombThrower 설정
	OnActorHit.AddDynamic(this, &AQLBomb::OnActorOverlap);
	bReplicates = true;
	SetReplicateMovement(true);
}


void AQLBomb::ThrowBomb(FVector Velocity)
{
	Collision->SetSimulatePhysics(true);

	Collision->SetPhysicsLinearVelocity(Velocity);
}

void AQLBomb::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Collision->SetSimulatePhysics(false);
	Mesh->SetSimulatePhysics(false);


	Collision->SetWorldLocation(Mesh->GetComponentLocation());
	OnActorOverlapDelegate.ExecuteIfBound();
}