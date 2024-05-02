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

	if (HasAuthority()) //World 위치하기 때문에 Role로 체크
	{
		for (const auto& ItemBox : ItemBoxClass)
		{
			int RandomCnt = FMath::RandRange(1.0f,ItemBox.Value); //int에 대해서 나오는 랜덤
			
			for (int Idx = 0; Idx < RandomCnt; Idx++)
			{
				FVector Location = GetActorLocation(); //Possessed Pawn Position
				Location.Z = 100.0f;
				FActorSpawnParameters Params;
				Params.Owner = this;
				AQLItemBox* Item = GetWorld()->SpawnActor<AQLItemBox>(ItemBox.Key, Location, FRotator::ZeroRotator, Params);
			}
		}
	}
	
}