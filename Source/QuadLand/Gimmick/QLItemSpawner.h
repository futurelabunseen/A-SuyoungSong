// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLItemSpawner.generated.h"

UCLASS()
class QUADLAND_API AQLItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLItemSpawner();

	void BeginPlay();
protected:
	UPROPERTY(EditAnywhere, Category = Item)
	TMap<TSubclassOf<class AQLItemBox>, float> ItemBoxClass; //int32 �� �ִ� ���ڸ� �ǹ���.

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;


	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCDestorySpawner();
};
