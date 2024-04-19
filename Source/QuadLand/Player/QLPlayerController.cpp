// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "UI/QLPlayerHUDWidget.h"
#include "UI/QLUserWidget.h"
#include "QuadLand.h"

AQLPlayerController::AQLPlayerController()
{
	ASC = nullptr;
}

void AQLPlayerController::SetInvisibleFarming()
{

	if (IsLocalController())
	{
		CrossHairUI->SetVisibility(ESlateVisibility::Hidden); //보이도록 함.
	}
}

UAbilitySystemComponent* AQLPlayerController::GetAbilitySystemComponent() const
{
	return ASC;
}

void AQLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (CrossHairHUDClass)
		{
			CrossHairUI = CreateWidget<UUserWidget>(this, CrossHairHUDClass);
			CrossHairUI->AddToViewport();
			CrossHairUI->SetVisibility(ESlateVisibility::Visible); //보이도록 함.
		}

		if (PlayerStatHUDClass)
		{
			StatUI = CreateWidget<UQLUserWidget>(this, PlayerStatHUDClass);
			StatUI->AddToViewport();
			StatUI->SetVisibility(ESlateVisibility::Visible); //보이도록 함.
		}

		CreateHUD();
	}
}

void AQLPlayerController::SetVisibleFarming()
{
	if (IsLocalController())
	{
		CrossHairUI->SetVisibility(ESlateVisibility::Visible); //보이도록 함.
	}
}

//Only Server
void AQLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

//Only Client
void AQLPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AQLPlayerController::CreateHUD()
{
	if (!CrossHairHUDClass || !PlayerStatHUDClass) return;

	if (!IsLocalPlayerController())
	{
		return;
	}
	
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	if (!PS)
	{
		QL_LOG(QLNetLog, Warning, TEXT("PlayerState is not founded"));
		return;
	}

	StatUI->UpdateAmmo(PS->GetAmmoCnt());
	StatUI->UpdateRemainingAmmo(0.0f); //임시값 삽입
	StatUI->UpdateHPPercentage(PS->GetHealth(), PS->GetMaxHealth());
	//HUD 초기화
}


/*
FVector2D ScreenPosition = YourHUDWidget->GetCachedGeometry().GetAbsolutePosition();
FVector WorldLocation, WorldDirection;

// 화면 좌표를 월드 좌표로 변환
APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection);


*/