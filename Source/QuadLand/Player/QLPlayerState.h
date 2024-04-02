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

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	//������ �̿��ϱ� ���� WeaponStat�� �����ؾ��ҵ� - ���θ��� �ٸ��� ������ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLWeaponStat> WeaponStat;

	friend class AQLCharacterPlayer;
};
