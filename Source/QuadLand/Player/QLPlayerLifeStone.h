// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/QLItem.h"
#include "QLPlayerLifeStone.generated.h"

UCLASS()
class QUADLAND_API AQLPlayerLifeStone : public AQLItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLPlayerLifeStone();
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class USphereComponent> Trigger;

	//UFUNCTION()
	//void OnBeginOverllap(class UPrimitiveComponent *HitComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult &SweepResult);
};
