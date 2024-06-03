// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "QLCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLCharacterNonPlayer : public AQLCharacterBase
{
	GENERATED_BODY()
	

public:

	AQLCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);
	class UAbilitySystemComponent* GetAbilitySystemComponent();
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

	void SetAiming(bool InAiming) { bIsAiming = InAiming; }
protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UQLAS_WeaponStat> WeaponStat; //TypeA

	UPROPERTY()
	TObjectPtr<class UQLAS_PlayerStat> PlayerStat; //PlayerStat

	UPROPERTY(EditAnywhere, Category = Weapon)
	TObjectPtr<class USkeletalMesh> GunMesh;

};
