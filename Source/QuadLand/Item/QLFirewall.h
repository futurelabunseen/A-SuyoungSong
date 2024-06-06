// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLFirewall.generated.h"

UCLASS()
class QUADLAND_API AQLFirewall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLFirewall();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void TakenDamage(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void StopDamage(AActor* Target);

protected:

	UPROPERTY(EditAnywhere, Category = GameplayEffect)
	TSubclassOf<class UGameplayEffect> DotDamageGameplayEffectClass;
};
