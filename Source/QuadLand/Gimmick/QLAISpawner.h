// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLAISpawner.generated.h"

UCLASS()
class QUADLAND_API AQLAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLAISpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = NonPlayer)
	TSubclassOf<class AQLCharacterNonPlayer> NonPlayerClass;
};
