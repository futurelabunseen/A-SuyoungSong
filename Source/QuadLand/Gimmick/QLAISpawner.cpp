// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLAISpawner.h"
#include "GameFramework/PlayerStart.h"
#include "Character/QLCharacterNonPlayer.h"
#include "EngineUtils.h"
#include "Game/QLGameMode.h"
#include "Physics/QLCollision.h"

// Sets default values
AQLAISpawner::AQLAISpawner()
{

}

// Called when the game starts or when spawned
void AQLAISpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		UWorld* CurrentWorld = GetWorld();
		AQLGameMode* GameMode = Cast<AQLGameMode>(CurrentWorld->GetAuthGameMode());

		for (const auto& Entry : FActorRange(CurrentWorld))
		{
			APlayerStart* PlayerStart = Cast<APlayerStart>(Entry);

			if (PlayerStart)
			{
				//PlayerStart 위치에서 라인트레이스를 쏘고, APawn이 없다면 그 위치에 Pawn 생성
				
				FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //식별자 

				FHitResult OutHitResult;

				FVector StartLocation = PlayerStart->GetActorLocation();

				FVector EndLocation = StartLocation + 150.0f * GetActorUpVector() * -1;
				bool bResult = GetWorld()->LineTraceSingleByChannel(
					OutHitResult,
					StartLocation,
					EndLocation,
					CCHANNEL_QLACTION,
					CollisionParams
				);

				if (bResult == false)
				{
					const FTransform SpawnTransform(StartLocation + FVector::UpVector * 88.0f);
					
					AQLCharacterNonPlayer* NonPlayer = GetWorld()->SpawnActorDeferred<AQLCharacterNonPlayer>(NonPlayerClass, SpawnTransform);
					
					if (NonPlayer)
					{
						NonPlayer->FinishSpawning(SpawnTransform);
					}

					if (GameMode)
					{
						GameMode->AddPlayer(FName(NonPlayer->GetName()));
					}
				}
				
			}
		}

		//게임모드를 가져온다.
		
	}
}
