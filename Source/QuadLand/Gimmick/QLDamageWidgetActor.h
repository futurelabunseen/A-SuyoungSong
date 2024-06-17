// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLDamageWidgetActor.generated.h"

UCLASS()
class QUADLAND_API AQLDamageWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLDamageWidgetActor();

	void SetDamage(const float &InDamage);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> DamageComponent;

	UFUNCTION(NetMulticast,Unreliable)
	void MulitcastRPCShowDamage(float InDamage);
};
