// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QLWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUADLAND_API UQLWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQLWeaponComponent();

	const class USkeletalMeshComponent* GetWeaponMesh() const { return Weapon; }

	bool GetIsReload() { return bIsReload; }
	bool GetIsShooting() { return bIsShooting; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimInstance> AnimInstance;

	uint8 bIsReload : 1;
	uint8 bIsShooting : 1;

	friend class AQLCharacterPlayer;
};
