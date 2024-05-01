// Fill out your copyright notice in the Description page of Project Settings.


#include "QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameData/QLWeaponStat.h"
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
	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f), FRotator(90.f, -90.f, 0.f)); //Weapon �����̱� ������ �����ۿ����� ���� ����
	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);

	OnActorHit.AddDynamic(this, &AQLItemBox::OnActorOverlap);
	bReplicates = true;
	SetReplicateMovement(true);
}

void AQLItemBox::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		float RandomImpulseX = FMath::FRandRange(-1000.0f, 1000.0f); // MaxImpulseX�� �ִ� ���� ũ��
		float RandomImpulseY = FMath::FRandRange(-1000.0f, 1000.0f); // MaxImpulseX�� �ִ� ���� ũ��

		// ������ ���� X�� �������� �����ϰ�, �ٸ� ���� 0���� �����մϴ�.
		FVector Impulse = FVector(RandomImpulseX, RandomImpulseY, 0.0f);
		Mesh->AddImpulse(Impulse);
	}
}


void AQLItemBox::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if(HasAuthority())
	{
		Trigger->SetSimulatePhysics(false);
		Mesh->SetSimulatePhysics(false);

		//UE_LOG(LogTemp, Warning, TEXT("Current Mesh Pos %s"), *Mesh->GetRelativeLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Current Trigger Pos %s"), *Trigger->GetRelativeLocation().ToString());

		Trigger->SetRelativeLocation(Mesh->GetRelativeLocation());
	}

}