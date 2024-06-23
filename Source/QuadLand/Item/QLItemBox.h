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

	void InitPosition(const FVector &Location);
	void SetPhysics();
	float GetZPos();

	virtual void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Alert, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> AlertComponent;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
