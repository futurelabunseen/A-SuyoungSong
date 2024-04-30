// Fill out your copyright notice in the Description page of Project Settings.


#include "QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameData/QLWeaponStat.h"
#include "Physics/QLCollision.h"

// Sets default values
AQLItemBox::AQLItemBox()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));

	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f), FRotator(90.f, -90.f, 0.f)); //Weapon 기준이기 때문에 아이템에서는 변경 예정
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	bReplicates = true;
}
