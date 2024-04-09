// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLItemBox.generated.h"

UCLASS()
class QUADLAND_API AQLItemBox : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AQLItemBox();

	UPROPERTY(EditAnywhere, Category = Stat)
	TObjectPtr<class UQLItemData> Stat;
protected:

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> Mesh;

};
