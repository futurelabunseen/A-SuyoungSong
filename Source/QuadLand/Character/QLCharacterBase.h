// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameData/QLTurningInPlaceType.h"
#include "Interface/AttackHitCheckInterface.h"
#include "QLCharacterBase.generated.h"

UCLASS()
class QUADLAND_API AQLCharacterBase : public ACharacter, public IAttackHitCheckInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQLCharacterBase(const FObjectInitializer& ObjectInitializer);

	uint8 bHasGun : 1;

	/*AI�� ���� ���, InputAction*/
	bool bIsUsingGun();
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE float GetCurrnetYaw() { return CurrentYaw; }
	FORCEINLINE float GetCurrentPitch() { return CurrentPitch; }
	FORCEINLINE const class UQLWeaponComponent* GetWeapon() const { return Weapon; }
	FORCEINLINE ETurningPlaceType GetTurningInPlaceType() const { return TurningInPlace; }

protected:
	UPROPERTY(Replicated)
	ECharacterAttackType CurrentAttackType; //Server�κ��� �����Ǿ����.
	virtual FGameplayTag GetCurrentAttackTag() const override;

	virtual void Tick(float DeltaSeconds) override;
	//Attack
protected:

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

	//Movement Section
protected:
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

	uint8 bIsAiming : 1;

	FORCEINLINE float CalculateSpeed();
};
