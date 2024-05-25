// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/QLItemBox.h"
#include "QLWeaponItemBox.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLWeaponItemBox : public AQLItemBox
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AQLItemBox> AmmoClass;

public:
	virtual void OverlapCheck(); 
};
