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
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetCollisionProfileName(TEXT("NoCollision"));

	RootComponent = Trigger;
}

// Called when the game starts or when spawned
void AQLItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) //World 위치하기 때문에 Role로 체크
	{
		float MaxX = GetActorLocation().X + Trigger->GetScaledBoxExtent().X;
		float MaxY = GetActorLocation().Y + Trigger->GetScaledBoxExtent().Y;

		float MinX = GetActorLocation().X - Trigger->GetScaledBoxExtent().X;
		float MinY = GetActorLocation().Y - Trigger->GetScaledBoxExtent().Y;
		if (HasAuthority()) //World 위치하기 때문에 Role로 체크
		{
			for (const auto& ItemBox : ItemBoxClass)
			{
				int RandomCnt = FMath::RandRange(1.0f, ItemBox.Value); //int에 대해서 나오는 랜덤

				for (int Idx = 0; Idx < RandomCnt; Idx++)
				{
					int X = FMath::RandRange(MinX, MaxX); //int에 대해서 나오는 랜덤
					int Y = FMath::RandRange(MinY, MaxY);
					FVector Location(X, Y, 100.0f); //Possessed Pawn Position
					FActorSpawnParameters Params;
					Params.Owner = this;

					AQLItemBox* Item = GetWorld()->SpawnActor<AQLItemBox>(ItemBox.Key, Location, FRotator::ZeroRotator, Params);
					Item->InitPosition(Location);
				}
			}
		}
	}
}

void AQLItemSpawner::MulticastRPCDestorySpawner_Implementation()
{
	SetLifeSpan(10.0f);
}
