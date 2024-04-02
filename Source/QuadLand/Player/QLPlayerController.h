// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AQLPlayerController();

	void SetVisibleFarming();
	void SetInvisibleFarming();
	const UUserWidget* GetCrossHairHUDWidget() const { return CrossHairHUD; }
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TSubclassOf<class UUserWidget> UserHUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TObjectPtr<class UUserWidget> CrossHairHUD;


	virtual void BeginPlay() override;
};
