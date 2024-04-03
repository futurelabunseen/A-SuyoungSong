// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/TA/QLTA_TraceResult.h"
#include "QLTA_LineTraceResult.generated.h"

/**
 * 
 */

UCLASS()
class QUADLAND_API AQLTA_LineTraceResult : public AQLTA_TraceResult
{
	GENERATED_BODY()
	
public:

	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;
};
