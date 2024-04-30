// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLItemSpawner.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Item/QLItemBox.h"
#include "Net/UnrealNetwork.h"
// Sets default values
AQLItemSpawner::AQLItemSpawner()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/MapTemplates/SM_Template_Map_Floor.SM_Template_Map_Floor'"));

	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.0, 775.0f, 300.0f));
	StageTrigger->SetupAttachment(Mesh);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	//StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

}

// Called when the game starts or when spawned
void AQLItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	for (const auto& ItemBox : ItemBoxClass)
	{
		AQLItemBox * Item= NewObject<AQLItemBox>(this);
		Item->SetReplicateMovement(true);
	}
	
}



