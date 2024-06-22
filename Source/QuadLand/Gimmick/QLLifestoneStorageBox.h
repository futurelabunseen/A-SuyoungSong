// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLLifestoneStorageBox.generated.h"

DECLARE_DELEGATE_TwoParams(FOnUpdateAlertPanel, FName, AQLLifestoneStorageBox*);
DECLARE_DELEGATE_OneParam(FOnLifestoneChangedDelegate, bool);
DECLARE_DELEGATE(FOnLifespanDelegate);
UCLASS()
class QUADLAND_API AQLLifestoneStorageBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLLifestoneStorageBox();

	virtual void Tick(float DeltaSeconds) override;

	bool GetAlreadyHidden() { return bIsAlreadyHidden; }
	void UpdateAlertPanel(FName InPlayerStateName);
	void RotateStorageBox();

	FORCEINLINE const FVector2D& GetStorageWidgetLocation() {
		return StorageWidgetLocation; 
	}
protected:

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class USphereComponent> Trigger;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	uint8 bIsAlreadyHidden : 1;
	
	FName PlayerStateName;
public:
	FOnLifespanDelegate OnLifespanDelegate;
	FOnLifestoneChangedDelegate OnLifestoneChangedDelegate;
	FOnUpdateAlertPanel OnUpdateAlertPanel;

	void ConcealLifeStone(FName InPlayerStateName,bool HasLifeStone) ;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	FVector2D StorageWidgetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Alert, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> AlertComponent;

protected:

	UPROPERTY()
	TArray<TWeakObjectPtr<class APlayerController>> PCs;
};
