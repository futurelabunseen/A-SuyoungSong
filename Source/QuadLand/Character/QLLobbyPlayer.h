// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QLLobbyPlayer.generated.h"

UCLASS()
class QUADLAND_API AQLLobbyPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AQLLobbyPlayer();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	
	virtual void BeginPlay();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SkipAction;

	void SkipOpening();


	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	uint8 bSkipOpening : 1;
};
