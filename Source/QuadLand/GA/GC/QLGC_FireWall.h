// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "QLGC_FireWall.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGC_FireWall : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UQLGC_FireWall();
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<class UParticleSystem> ParticleSystem;
///Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Fire/P_Fire_Wall.P_Fire_Wall'
};
