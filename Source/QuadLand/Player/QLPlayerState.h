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
	
	void SetAmmoStat(float AmmoCnt);
	void SetWeaponStat(const class UQLWeaponStat* Stat);
	void AddHPStat(float HP);
	void AddStaminaStat(float Stamina);
	void ResetWeaponStat(const class UQLWeaponStat* Stat);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsWin() { return bIsWin; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbIsDead() { return bIsDead; }

	float GetStamina();
	float GetMaxStamina();
	float GetHealth();
	float GetMaxHealth();
	float GetCurrentAmmoCnt();
	float GetMaxAmmoCnt();

	FORCEINLINE void SetHasLifeStone(bool InHasLifeStone) { bHasLifeStone = InHasLifeStone; }
	FORCEINLINE bool GetHasLifeStone() { return bHasLifeStone; }

	UFUNCTION()
	virtual void Win(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	virtual void Dead(const FGameplayTag CallbackTag, int32 NewCount);

	void SetDead();
	virtual void BeginPlay() override;
protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_PlayerStat> PlayerStatInfo;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAS_WeaponStat> WeaponStatInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //���ø����̼� ���
protected:

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bIsWin : 1;

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bIsDead : 1;

	//void SetHasLifeStone(bool InGetStone) { bHasLifeStone = true; }
	//HUD Update Section
protected:

	FDelegateHandle HealthChangedDeleagteHandle;
	FDelegateHandle MaxHealthChangedDeleagteHandle;
	FDelegateHandle AmmoChangedDeleagteHandle;
	FDelegateHandle MaxAmmoChangedDeleagteHandle;
	FDelegateHandle StaminaChangedDeleagteHandle;
	FDelegateHandle MaxStaminaChangedDeleagteHandle;


	virtual void OnChangedStamina(const FOnAttributeChangeData& Data);

	virtual void OnChangedMaxStamina(const FOnAttributeChangeData& Data);

	virtual void OnChangedHp(const FOnAttributeChangeData& Data);

	virtual void OnChangedMaxHp(const FOnAttributeChangeData& Data); //���� �ִ� �Ѿ� ������ ������ -> 'R'eload �����ϰ� �� ����

	virtual void OnChangedAmmoCnt(const FOnAttributeChangeData& Data);

	virtual void OnChangedMaxAmmoCnt(const FOnAttributeChangeData& Data);
//LifeStone Section
protected:

	UFUNCTION(Server, Reliable)
	void ServerRPCPutLifeStone(); //�������� �������� ����

	UFUNCTION(Server, Reliable)
	void ServerRPCConcealLifeStone();

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bHasLifeStone : 1;

	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	TObjectPtr<class AQLPlayerLifeStone> LifeStone;

	friend class AQLCharacterPlayer;
};
