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

	class UStaticMeshComponent* GetMesh() const { return Mesh; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class USphereComponent> Trigger;

	UFUNCTION()
	void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

};
