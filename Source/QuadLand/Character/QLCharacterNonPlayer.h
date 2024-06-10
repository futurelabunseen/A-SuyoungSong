// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "QLCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLCharacterNonPlayer : public AQLCharacterBase, public IQLLifestoneContainerInterface
{
	GENERATED_BODY()
	
public:

	AQLCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	virtual FRotator GetBaseAimRotation() const override;
	void AttachTakeDamageTag(const FGameplayTag CallbackTag, int32 NewCount);
	
	void Dead(const FGameplayTag CallbackTag, int32 NewCount);
	bool CanTakeDamage();
	void StopDamage();

	FVector2D GetXRandOffset() { return XRandOffset; }

	FVector2D GetYRandOffset() { return YRandOffset; }
protected:

	uint8 bTakeDamage : 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = AI)
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UPROPERTY()
	TObjectPtr<class UQLAS_WeaponStat> WeaponStat; //TypeA

	UPROPERTY()
	TObjectPtr<class UQLAS_PlayerStat> PlayerStat; //PlayerStat

	UPROPERTY(EditAnywhere, Category = Weapon)
	TObjectPtr<class USkeletalMesh> GunMesh;

	virtual void CheckBoxOverlap() override;

	UFUNCTION()
	void UpdateTargetPerception(AActor* Actor, FAIStimulus Stimulus);

	FTimerHandle CheckTargetTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackOffset)
	FVector2D XRandOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackOffset)
	FVector2D YRandOffset;

	void ChangeTarget();

};
