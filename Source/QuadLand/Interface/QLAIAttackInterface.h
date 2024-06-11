// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QLAIAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UQLAIAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QUADLAND_API IQLAIAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual const APawn *GetTarget() = 0;
	virtual FVector GetTargetPos() = 0;
	virtual void SetTargetPos() = 0;
};
