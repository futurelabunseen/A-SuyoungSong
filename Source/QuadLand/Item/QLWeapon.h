// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/QLItemObject.h"
#include "GameFramework/Actor.h"
#include "Interface/ItemGettingInfoInterface.h"
#include "QLWeapon.generated.h"

UCLASS()
class QUADLAND_API AQLWeapon : public AQLItemObject, public IItemGettingInfoInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLWeapon();

	virtual class UStaticMeshComponent* GetMesh() const override;
	virtual class UQLWeaponStat* GetStat() const override;
protected:

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = Stat)
	TObjectPtr<class UQLWeaponStat> WeaponStat;
};
