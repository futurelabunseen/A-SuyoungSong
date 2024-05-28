// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLItemSpawner_V2.h"
#include "Item/QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Physics/QLCollision.h"
#include "CollisionQueryParams.h"
#include "QuadLand.h"

// Sets default values
AQLItemSpawner_V2::AQLItemSpawner_V2()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetCollisionProfileName(TEXT("NoCollision"));

}

// Called when the game starts or when spawned
void AQLItemSpawner_V2::BeginPlay()
{
	Super::BeginPlay();
	
	float MaxX = GetActorLocation().X + Trigger->GetScaledBoxExtent().X;
	float MaxY = GetActorLocation().Y + Trigger->GetScaledBoxExtent().Y;

	float MinX = GetActorLocation().X;
	float MinY = GetActorLocation().Y;
	if (HasAuthority()) //World ��ġ�ϱ� ������ Role�� üũ
	{
		for (const auto& ItemBox : ItemBoxClass)
		{
			int RandomCnt = FMath::RandRange(1.0f, ItemBox.Value); //int�� ���ؼ� ������ ����

			for (int Idx = 0; Idx < RandomCnt; Idx++)
			{
				int X = FMath::RandRange(MinX, MaxX); //int�� ���ؼ� ������ ����
				int Y = FMath::RandRange(MinY, MaxY);
				FVector Location(X, Y, 100.0f); //Possessed Pawn Position
				FActorSpawnParameters Params;
				Params.Owner = this;

				AQLItemBox* Item = GetWorld()->SpawnActor<AQLItemBox>(ItemBox.Key, Location, FRotator::ZeroRotator, Params);
				FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //�ĺ��� 

				FHitResult OutHitResult;

				FVector StartLocation = Location;

				FVector EndLocation = StartLocation + 150.0f * Item->GetActorUpVector() * -1;
				bool bResult = GetWorld()->LineTraceSingleByChannel(
					OutHitResult,
					StartLocation,
					EndLocation,
					CCHANNEL_QLGROUND,
					CollisionParams
				);

				if (bResult)
				{
					QL_LOG(QLLog, Log, TEXT("Location %s"), *OutHitResult.Location.ToString());
					OutHitResult.Location.Z += Item->GetZPos();
					Item->SetActorLocation(OutHitResult.Location);
				}
			}
		}
	}

}
