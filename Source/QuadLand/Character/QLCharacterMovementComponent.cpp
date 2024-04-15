// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterMovementComponent.h"
#include "GameFramework/Character.h"
UQLCharacterMovementComponent::UQLCharacterMovementComponent()
{
	SprintMaxSpeed = 600.0f;
	SprintMinSpeed = 400.0f;

	bPressedSprint = false;
}

void UQLCharacterMovementComponent::SetSprintCommand()
{
	bPressedSprint = true; //눌림
}

void UQLCharacterMovementComponent::UnSetSprintCommand()
{
	bPressedSprint = false; //눌림 해제
}


void UQLCharacterMovementComponent::RunOrWalk()
{
	if (CharacterOwner)
	{
		if (bPressedSprint)
		{
			MaxWalkSpeed = SprintMaxSpeed;
		}
		else
		{
			MaxWalkSpeed = SprintMinSpeed;
		}
	}
}

void UQLCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	RunOrWalk();
}

void UQLCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bPressedSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		//클라이언트에서 동작했으니 서버에서도 동작
		
		RunOrWalk();
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
}

void FQLSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	FSavedMove_Character::SetInitialPosition(Character);

	UQLCharacterMovementComponent* QLMovement = Cast<UQLCharacterMovementComponent>(Character->GetCharacterMovement());

	if (QLMovement)
	{
		bPressedSprint = QLMovement->bPressedSprint;
	}

}

uint8 FQLSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	
	if (bPressedSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}
