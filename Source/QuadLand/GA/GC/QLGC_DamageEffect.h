// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "QLGC_DamageEffect.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGC_DamageEffect : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UQLGC_DamageEffect();

	/** Called when a GameplayCue is executed, this is used for instant effects or periodic ticks */
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayCue)
	TObjectPtr<class UMaterialInterface> BloodDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayCue)
	TObjectPtr<class UMaterialInterface> BulletMarks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<class UParticleSystem> ParticleSystem;

};
