// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "QLPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AQLPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void SetWeaponStat(class UQLWeaponStat *Stat);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsWin() { return bIsWin; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsDead() { return bIsDead; }

	UFUNCTION()
	virtual void Win(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	virtual void Dead(const FGameplayTag CallbackTag, int32 NewCount);

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_PlayerStat> PlayerStatInfo;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_WeaponStat> WeaponStatInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //리플리케이션 등록
protected:

	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	uint8 bIsWin : 1;

	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	uint8 bIsDead : 1;


	friend class AQLCharacterPlayer;
};
