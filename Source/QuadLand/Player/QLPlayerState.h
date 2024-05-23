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
	void BulletWaste(float DiscardedCount);
	void SetWeaponStat(const class UQLWeaponStat* Stat);
	void AddHPStat(float HP);
	void AddStaminaStat(float Stamina);
	void ResetWeaponStat(const class UQLWeaponStat* Stat);
	void UseGlassesItem(float Time);

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

	UFUNCTION()

	virtual void CoolTimeStamina(const FGameplayTag CallbackTag, int32 NewCount);

	FTimerHandle CoolTimer;

	void SetDead();
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerRPCConcealLifeStone();

	UFUNCTION(Server, Reliable)
	void ServerRPCPutLifeStone(); //서버에게 눌렀음을 전달
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

	virtual void OnChangedMaxHp(const FOnAttributeChangeData& Data); //남아 있는 총알 개수가 있으면 -> 'R'eload 가능하게 할 예정

	virtual void OnChangedAmmoCnt(const FOnAttributeChangeData& Data);

	virtual void OnChangedMaxAmmoCnt(const FOnAttributeChangeData& Data);
//LifeStone Section
protected:

	void UpdateStorageWidget(FName Nickname, class AQLLifestoneStorageBox* StorageBox);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdateStorageWidget(FName Nickname, class AQLLifestoneStorageBox* StorageBox);



	UPROPERTY(Replicated, EditAnywhere, Category = Battle)
	uint8 bHasLifeStone : 1;

	UPROPERTY(EditAnywhere, Category = Battle)
	TObjectPtr<class AQLPlayerLifeStone> LifeStone;

	UPROPERTY(EditAnywhere, Category = Battle)
	TSubclassOf<class AQLPlayerLifeStone> LifeStoneClass;
	friend class AQLCharacterPlayer;
};
