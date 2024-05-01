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

	class USkeletalMeshComponent* GetMesh() const { return Mesh; }
protected:

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> Mesh;
	
	
	UPROPERTY(EditAnywhere,Category = Position)
	float Radius; //ÁÂÇ¥°ªÀº cos, sin

	UPROPERTY(EditAnywhere, Category = Power)
	float Power; //ÁÂÇ¥°ªÀº cos, sin



	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay();

	void InitPosition();
	UFUNCTION()
	void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
