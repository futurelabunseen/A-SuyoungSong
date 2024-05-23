// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "QLGC_DamageEffectUsingPunch.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGC_DamageEffectUsingPunch : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UQLGC_DamageEffectUsingPunch();
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<class USoundWave> Sound;

};
