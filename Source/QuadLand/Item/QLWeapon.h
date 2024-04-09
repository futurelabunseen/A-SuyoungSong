// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "QLWeapon.generated.h"

UCLASS()
class QUADLAND_API AQLWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLWeapon();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UQLAS_WeaponStat> Stat;

	friend class AQLPlayerState;
};
