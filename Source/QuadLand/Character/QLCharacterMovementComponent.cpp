// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterMovementComponent.h"
#include "GameFramework/Character.h"
UQLCharacterMovementComponent::UQLCharacterMovementComponent()
{
	SprintMaxSpeed = 600.0f;
	SprintMinSpeed = 400.0f;
	ProneMinSpeed = 150.0f;

	bPressed = false;
	bPressedProne = false;
}

void UQLCharacterMovementComponent::ChangeSprintSpeedCommand()
{
	bPressed = true; //눌림
}

void UQLCharacterMovementComponent::RestoreSprintSpeedCommand()
{
	bPressed = false; //눌림 해제
}

void UQLCharacterMovementComponent::ChangeProneSpeedCommand()
{
	bPressedProne = true;
}

void UQLCharacterMovementComponent::RestoreProneSpeedCommand()
{
	bPressedProne = false;
}

void UQLCharacterMovementComponent::SpeedSetting()
{
	if (CharacterOwner)
	{

		if (bPressedProne == false && bPressed == true)
		{
			MaxWalkSpeed = SprintMaxSpeed;
		}
		else if (bPressedProne == true && bPressed == false)
		{
			MaxWalkSpeed = 150.0f;
		}
		else
		{
			MaxWalkSpeed = SprintMinSpeed;
		}
	}
}

void UQLCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	SpeedSetting();
}

void UQLCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bPressed = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bPressedProne = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		//클라이언트에서 동작했으니 서버에서도 동작
		SpeedSetting();
	}
}


FNetworkPredictionData_Client* UQLCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UQLCharacterMovementComponent* MutableThis = const_cast<UQLCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new QLFNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}

QLFNetworkPredictionData_Client_Character::QLFNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	:FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr QLFNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FQLSavedMove_Character());
}

void FQLSavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();
	bPressedSprint = false;
	bPressedProne = false;
}

void FQLSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	FSavedMove_Character::SetInitialPosition(Character);

	UQLCharacterMovementComponent* QLMovement = Cast<UQLCharacterMovementComponent>(Character->GetCharacterMovement());

	if (QLMovement)
	{
		bPressedSprint = QLMovement->bPressed;

		bPressedProne = QLMovement->bPressedProne;
	}

}

uint8 FQLSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	
	if (bPressedSprint)
	{
		Result |= FLAG_Custom_0;
	}

	if (bPressedProne)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}
