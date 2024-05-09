// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/WeaponType.h"
#include "QLWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUADLAND_API UQLWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQLWeaponComponent();

	const class USkeletalMeshComponent* GetWeaponMesh() const { return Weapon; }
	
	const class UQLWeaponStat* GetStat(EWeaponType Type);
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	//위를 아래로 교체예정
	UPROPERTY(EditAnywhere, Category = Stat)
	TMap<EWeaponType, TObjectPtr<class UQLWeaponStat>> Weapons;

	//UPROPERTY(EditAnywhere, Category = Stat)
	//TObjectPtr<class UQLWeaponStat> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLItemBox> GroundWeapon;

	friend class AQLCharacterPlayer;
};
