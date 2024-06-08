// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLSpectatorPawn.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AQLSpectatorPawn::AQLSpectatorPawn()
{
	//InputContext Mapping
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContextMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/QuadLand/Inputs/IMC_Spectator.IMC_Spectator'"));

	if (InputContextMappingRef.Object)
	{
		InputMappingContext = InputContextMappingRef.Object;
	}

	bReplicates = true;
}

void AQLSpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	if (const auto PlayerController = Cast<APlayerController>(GetController()))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}
