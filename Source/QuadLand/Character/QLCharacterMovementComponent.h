// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "QLCharacterMovementComponent.generated.h"

/**
 * 
 */

class FQLSavedMove_Character : public FSavedMove_Character
{
public:
	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;
	/** Set the properties describing the position, etc. of the moved pawn at the start of the move. */
	virtual void SetInitialPosition(ACharacter* C) override; //최초 위치, 최초 상태를 저장 
	virtual uint8 GetCompressedFlags() const override;

	uint8 bPressedSprint : 1; //입력 전달
	uint8 bPressedCrouch : 1;
};
class QLFNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character //상속받아서 구현 
{
public:
	QLFNetworkPredictionData_Client_Character(const UCharacterMovementComponent &ClientMovement);

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class QUADLAND_API UQLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UQLCharacterMovementComponent();

	void SetSprintCommand();
	void UnSetSprintCommand();

public:

	uint8 bPressedSprint : 1; //입력 전달

protected:
	//Sprint Max 값, Min 값
	UPROPERTY()
	float SprintMaxSpeed;

	UPROPERTY()
	float SprintMinSpeed;

	virtual void RunOrWalk();
protected:
	//Client Section
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
protected:
	//Server Section
	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags); 


};
