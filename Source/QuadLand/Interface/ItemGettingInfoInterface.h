// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemGettingInfoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemGettingInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QUADLAND_API IItemGettingInfoInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual class UStaticMeshComponent* GetMesh() const = 0;
	virtual class UQLWeaponStat* GetStat() const = 0;

};
