// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLBomb.generated.h"

DECLARE_DELEGATE(FOnActorOverlapDelegate);
UCLASS()
class QUADLAND_API AQLBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLBomb();

	void ThrowBomb(FVector Velocity);

	FORCEINLINE float GetAttackRange() { return AttackRange; }
	FORCEINLINE float GetAttackSpeed() { return Speed; }
	FORCEINLINE float GetDamage() { return Damage; }


	FORCEINLINE void SetAttackRange(float Range) 
	{
		AttackRange = Range;
	}
	FORCEINLINE void SetAttackSpeed(float InSpeed)
	{
		Speed = InSpeed;
	}

	FORCEINLINE void SetDamage(float InDamage)
	{
		Damage = InDamage;
	}

	//Attack
protected:
	void PlayGameplayCue(const class AQLCharacterBase* Character);
	void ExplodesBomb(class AQLCharacterBase* Character);
	void SpawnFire(const AQLCharacterBase* Character);
	FVector TargetLoc;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> AttackDamageEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bomb)
	TSubclassOf<class AQLFirewall> Fire;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Mesh;
	//이제 Stat은 가질필요없음 -> DataManager가 가지고 있기 때문에

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;

	UFUNCTION()
	void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	float AttackRange;
	float Speed;
	float Damage;
};
