// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLBomb.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
AQLBomb::AQLBomb()
{

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/QuadLand/Item3D/Bomb/Pipe_Bomb/Pipe_Bomb.Pipe_Bomb'"));

	if (MeshRef.Object)
	{
		Weapon->SetSkeletalMesh(MeshRef.Object);
	}

	RootComponent = Weapon;
}


