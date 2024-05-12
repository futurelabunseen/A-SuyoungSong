// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLMapBoundary.h"

// Sets default values
AQLMapBoundary::AQLMapBoundary()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	Mesh->Mobility = EComponentMobility::Static;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/BlackAlder/Geometry/SimpleWind/SM_BlackAlder_Forest_04.SM_BlackAlder_Forest_04'"));

	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	///Script/Engine.StaticMesh'/Game/VikingVillage/StaticMeshes/SM_woodWall.SM_woodWall'
}
