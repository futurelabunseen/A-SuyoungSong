// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
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
	
	void SetWeaponStat(const class UQLWeaponStat *Stat);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsWin() { return bIsWin; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsDead() { return bIsDead; }

	float GetHealth();
	float GetMaxHealth();
	float GetAmmoCnt();

	UFUNCTION()
	virtual void Win(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	virtual void Dead(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void BeginPlay() override;
protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_PlayerStat> PlayerStatInfo;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_WeaponStat> WeaponStatInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //리플리케이션 등록
protected:

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bIsWin : 1;

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bIsDead : 1;

	//HUD Update Section
protected:

	FDelegateHandle HealthChangedDeleagteHandle;
	FDelegateHandle MaxHealthChangedDeleagteHandle;
	FDelegateHandle AmmoChangedDeleagteHandle;

	virtual void OnChangedHp(const FOnAttributeChangeData& Data);

	virtual void OnChangedMaxHp(const FOnAttributeChangeData& Data); //남아 있는 총알 개수가 있으면 -> 'R'eload 가능하게 할 예정

	virtual void OnChangedAmmoCnt(const FOnAttributeChangeData& Data);
	
	friend class AQLCharacterPlayer;
};
