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

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	friend class AQLCharacterPlayer;
};
