// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/WeaponType.h"
#include "QLWeaponComponent.generated.h"


DECLARE_DELEGATE(FOnDestoryBomb);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUADLAND_API UQLWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQLWeaponComponent();

	const class USkeletalMeshComponent* GetWeaponMesh() const { return Weapon; }
	class UQLObjectPooling* GetObjectPoolingManager();
	void SpawnBomb();
	FVector GetMuzzlePos();
	void SetBombHiddenInGame(bool InHiddenInGame);
	void ResetBomb();
protected:

	UPROPERTY()
	TObjectPtr<class UQLObjectPooling> AmmoObjectPooling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AQLItemBox> GroundWeapon;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AQLBomb> Bomb;
	
	friend class AQLCharacterBase;
	friend class AQLCharacterPlayer;
	friend class AQLCharacterNonPlayer;

};
