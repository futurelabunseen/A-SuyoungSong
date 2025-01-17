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

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MenuAction;

	void ShowMenuUI();

	uint8 bShowMenuUI : 1;
};
