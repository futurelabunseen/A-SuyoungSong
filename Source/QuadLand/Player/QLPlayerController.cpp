// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"

AQLPlayerController::AQLPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidgetRef(TEXT("/Game/QuadLand/UI/WBQL_Crosshair.WBQL_Crosshair_C"));

	if (UserWidgetRef.Class)
	{
		UserHUDWidgetClass = UserWidgetRef.Class;
	}
}

void AQLPlayerController::SetInvisibleFarming()
{

	if (IsLocalController())
	{
		CrossHairHUD->SetVisibility(ESlateVisibility::Hidden); //���̵��� ��.
	}
}

void AQLPlayerController::SetVisibleFarming()
{
	if (IsLocalController())
	{
		CrossHairHUD->SetVisibility(ESlateVisibility::Visible); //���̵��� ��.
	}
}

void AQLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (UserHUDWidgetClass)
		{
			CrossHairHUD = CreateWidget<UUserWidget>(this, UserHUDWidgetClass);
		}
		CrossHairHUD->AddToViewport();
		CrossHairHUD->SetVisibility(ESlateVisibility::Visible); //���̵��� ��.
	}
}


/*
FVector2D ScreenPosition = YourHUDWidget->GetCachedGeometry().GetAbsolutePosition();
FVector WorldLocation, WorldDirection;

// ȭ�� ��ǥ�� ���� ��ǥ�� ��ȯ
APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection);


*/