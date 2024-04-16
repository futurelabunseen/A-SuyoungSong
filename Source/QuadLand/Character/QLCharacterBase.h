// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/QLReceivedDamageInterface.h"
#include "Interface/AttackHitCheckInterface.h"
#include "QLCharacterBase.generated.h"

UCLASS()
class QUADLAND_API AQLCharacterBase : public ACharacter, public IAttackHitCheckInterface, public IQLReceivedDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQLCharacterBase(const FObjectInitializer& ObjectInitializer);

	uint8 bHasGun : 1;

	/*AI와 같이 사용, InputAction*/
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

protected:
	UPROPERTY(Replicated)
	ECharacterAttackType CurrentAttackType; //Server로부터 복제되어야함.
	virtual FGameplayTag GetCurrentAttackTag() const override;
	//Attack
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//TMap으로 변경하자. Gun 없을 때 Montage와 Gun 있을 때 Montage 로 나누어서 관리 - enum으로 분류 예정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TMap<ECharacterAttackType, TObjectPtr<class UAnimMontage>> AttackAnimMontage;

};
