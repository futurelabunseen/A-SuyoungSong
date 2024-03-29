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

void AQLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UserHUDWidgetClass)
	{
		UserHUDWidget = CreateWidget<UUserWidget>(this, UserHUDWidgetClass);
	}
	UserHUDWidget->AddToViewport();
	UserHUDWidget->SetVisibility(ESlateVisibility::Visible); //보이도록 함.

}


/*
FVector2D ScreenPosition = YourHUDWidget->GetCachedGeometry().GetAbsolutePosition();
FVector WorldLocation, WorldDirection;

// 화면 좌표를 월드 좌표로 변환
APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection);


*/