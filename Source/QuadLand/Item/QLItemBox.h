// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/QLItem.h"
#include "QLItemBox.generated.h"

UCLASS()
class QUADLAND_API AQLItemBox : public AQLItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AQLItemBox();
protected:

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> Mesh;

};
