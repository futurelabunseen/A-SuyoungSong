// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "QLSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	AQLSpectatorPawn();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
};
