// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLWeapon.generated.h"

UCLASS()
class QUADLAND_API AQLWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLWeapon();

	const class USkeletalMeshComponent* GetWeaponMesh() const { return Weapon; }
	const class UQLWeaponStat* GetStat() const { return Stat; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = Stat)
	TObjectPtr<class UQLWeaponStat> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLItemBox> GroundWeapon;

	friend class AQLCharacterPlayer;

};
