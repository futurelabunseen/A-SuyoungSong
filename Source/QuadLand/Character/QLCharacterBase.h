// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameData/QLTurningInPlaceType.h"
#include "Interface/AttackHitCheckInterface.h"
#include "AbilitySystemInterface.h"
#include "QLCharacterBase.generated.h"

UCLASS()
class QUADLAND_API AQLCharacterBase : public ACharacter, public IAttackHitCheckInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQLCharacterBase(const FObjectInitializer& ObjectInitializer);

	uint8 bHasGun : 1;

	/*AI�� ���� ���, InputAction*/
	bool bIsUsingGun();
	FORCEINLINE float GetCurrnetYaw() { return CurrentYaw; }
	FORCEINLINE float GetCurrentPitch() { return CurrentPitch; }
	FORCEINLINE const class UQLWeaponComponent* GetWeapon() const { return Weapon; }
	FORCEINLINE ETurningPlaceType GetTurningInPlaceType() const { return TurningInPlace; }
	virtual FGameplayTag GetCurrentAttackTag() const override;
	//FORCEINLINE
	class UAnimMontage* GetAnimMontage() const
	{
		return AttackAnimMontage[CurrentAttackType];
	}
	UFUNCTION(Server, Unreliable)
	void ServerRPCShooting(); //ȿ�����̱� ������ ���� Reliable �� �ʿ� ����.

	FORCEINLINE bool GetIsShooting() const { return bIsShooting; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsReload() const { return bIsReload; }
	FORCEINLINE void SetIsReload(bool Reload) { bIsReload = Reload; }
	FORCEINLINE void SetIsDead(bool InDead) { bIsDead = InDead; }
	FORCEINLINE bool GetIsCrunching() const { return bIsCrouched; }
	UFUNCTION(Server, Reliable)
	void ServerRPCReload(); //Reload ������ Reliable
	FVector GetWeaponMuzzlePos();

	//ASC
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(Replicated)
	uint8 bIsShooting : 1;

	UPROPERTY(Replicated)
	uint8 bIsReload : 1;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Aim)
	uint8 bIsAiming : 1;

protected:
	UPROPERTY(Replicated)
	ECharacterAttackType CurrentAttackType; //Server�κ��� �����Ǿ����.

	virtual void Tick(float DeltaSeconds) override;
	//Attack
protected:
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly, Category = Battle)
	uint8 bIsDead : 1;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//TMap���� ��������. Gun ���� �� Montage�� Gun ���� �� Montage �� ����� ���� - enum���� �з� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TMap<ECharacterAttackType, TObjectPtr<class UAnimMontage>> AttackAnimMontage;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLWeaponComponent> Weapon;

protected:
	//Movement Section
	FRotator PreviousRotation;

	//Turning in Place Section
protected:
	ETurningPlaceType TurningInPlace;

	void RotateBornSetting(float DeltaTime);
	void TurnInPlace(float DeltaTime);

	float InterpYaw; //�����뵵
	float CurrentYaw;
	float CurrentPitch;

protected:

	FORCEINLINE float CalculateSpeed();

public:

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCGunAttackAnimMont();
};
