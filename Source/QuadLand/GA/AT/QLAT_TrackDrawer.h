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

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** Wait until the user releases the input button for this ability's activation. Returns time from hitting this node, till release. Will return 0 if input was already released. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UQLAT_TrackDrawer* CreateTask(UGameplayAbility* OwningAbility, class UStaticMesh* InDrawer, bool bTestAlreadyReleased = false);

	void Recursive();

protected:

	FTimerHandle DrawerTimerHandle;

	UPROPERTY()
	TObjectPtr<class AQLCharacterPlayer> Character;

	UPROPERTY()
	TObjectPtr<class USplineComponent> BombPath;

	UPROPERTY()
	TArray<TObjectPtr<class USplineMeshComponent>> BombPathMeshComp;

	UPROPERTY()
	TObjectPtr<class UStaticMesh> StaticMesh;

	FVector PreForward;
};
