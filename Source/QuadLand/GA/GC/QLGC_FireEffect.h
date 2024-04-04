// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "QLGC_FireEffect.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGC_FireEffect : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UQLGC_FireEffect();

	/** Called when a GameplayCue is executed, this is used for instant effects or periodic ticks */
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayCue)
	TObjectPtr<class UParticleSystem> FireParticleEffect;
};
