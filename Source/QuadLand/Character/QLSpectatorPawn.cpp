// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLSpectatorPawn.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Player/QLPlayerController.h"

AQLSpectatorPawn::AQLSpectatorPawn()
{
	//InputContext Mapping
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContextMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/QuadLand/Inputs/IMC_Spectator.IMC_Spectator'"));

	if (InputContextMappingRef.Object)
	{
		InputMappingContext = InputContextMappingRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Menu.IA_Menu'"));

	if (MenuActionRef.Object)
	{
		MenuAction = MenuActionRef.Object;
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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Completed, this, &AQLSpectatorPawn::ShowMenuUI);
}

void AQLSpectatorPawn::ShowMenuUI()
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (PC == nullptr)
	{
		return;
	}

	if (bShowMenuUI == false)
	{
		PC->SetVisibilityHUD(EHUDType::Menu);
		PC->bShowMouseCursor = true;
		bShowMenuUI = true;
	}
	else
	{
		PC->CloseHUD(EHUDType::Menu);
		PC->SetHiddenHUD(EHUDType::Menu);
		PC->bShowMouseCursor = false;
		bShowMenuUI = false;
	}

}