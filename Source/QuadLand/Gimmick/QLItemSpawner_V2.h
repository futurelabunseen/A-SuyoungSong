// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLItemSpawner_V2.generated.h"

UCLASS()
class QUADLAND_API AQLItemSpawner_V2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLItemSpawner_V2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = Item)
	TMap<TSubclassOf<class AQLItemBox>, float> ItemBoxClass; //int32 는 최대 숫자를 의미함.
	
	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

};
