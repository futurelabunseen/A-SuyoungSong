// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLMapBoundary.generated.h"

UCLASS()
class QUADLAND_API AQLMapBoundary : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLMapBoundary();

	FORCEINLINE FVector GetLocation() { return GetActorLocation(); }
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Mesh)
	FString RatioCalculatorUI;
	
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Mesh;
};
