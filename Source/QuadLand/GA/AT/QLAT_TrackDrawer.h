// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "QLAT_TrackDrawer.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAT_TrackDrawer : public UAbilityTask_WaitInputRelease
{
	GENERATED_BODY()
	
public:
	UQLAT_TrackDrawer();

	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;

	/** Wait until the user releases the input button for this ability's activation. Returns time from hitting this node, till release. Will return 0 if input was already released. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UQLAT_TrackDrawer* CreateTask(UGameplayAbility* OwningAbility, bool bTestAlreadyReleased = false);

protected:
	UPROPERTY()
	TObjectPtr<class AQLCharacterPlayer> Character;

	UPROPERTY()
	TObjectPtr<class AQLBomb> Bomb;

	FVector PreForward;
};
