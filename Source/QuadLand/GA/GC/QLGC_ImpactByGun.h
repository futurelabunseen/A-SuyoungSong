// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "QLGC_ImpactByGun.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGC_ImpactByGun : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UQLGC_ImpactByGun();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Trace")
	TObjectPtr<class AActor> Trace;
};
