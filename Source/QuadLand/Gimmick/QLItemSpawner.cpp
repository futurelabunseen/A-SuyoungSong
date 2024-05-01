// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLItemSpawner.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Item/QLItemBox.h"
#include "Net/UnrealNetwork.h"
#include "QuadLand.h"
// Sets default values
AQLItemSpawner::AQLItemSpawner()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	Mesh->Mobility = EComponentMobility::Static;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/MapTemplates/SM_Template_Map_Floor.SM_Template_Map_Floor'"));

	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AQLItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	QL_LOG(QLNetLog, Log, TEXT("Current ownership"));
	if (HasAuthority()) //World 위치하기 때문에 Role로 체크
	{
		for (const auto& ItemBox : ItemBoxClass)
		{
			FVector Location = GetActorLocation(); //Possessed Pawn Position
			Location.Z = 100.0f;
			FActorSpawnParameters Params;
			Params.Owner = this;
			AQLItemBox* Item = GetWorld()->SpawnActor<AQLItemBox>(ItemBox.Key,Location, FRotator::ZeroRotator, Params);
		}
	}
	
}