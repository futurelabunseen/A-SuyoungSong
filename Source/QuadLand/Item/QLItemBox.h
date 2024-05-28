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

	AQLItemBox();

	class USkeletalMeshComponent* GetMesh() const { return Mesh; }

	void InitPosition();
	float GetZPos();
	UPROPERTY(EditAnywhere, Category = Position)
	float Radius; //ÁÂÇ¥°ªÀº cos, sin

	UPROPERTY(EditAnywhere, Category = Power)
	float Power; //ÁÂÇ¥°ªÀº cos, sin

protected:

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> Mesh;

	UFUNCTION()
	void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
