// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLFirewall.h"
#include "Character/QLCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "QuadLand.h"
// Sets default values
AQLFirewall::AQLFirewall()
{
}

void AQLFirewall::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.0f); //크기가 줄어들면서 위력도 약해져야하나
}

void AQLFirewall::TakenDamage(AActor* Target)
{
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Target);
	if (Player)
	{
		UAbilitySystemComponent* TargetASC = Cast<UAbilitySystemComponent>(Player->GetAbilitySystemComponent());
		
		if (TargetASC)
		{
			//도트 적용
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(DotDamageGameplayEffectClass, 1, EffectContext);

			if (EffectSpecHandle.IsValid())
			{
				TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
			}
		}
	}

	QL_LOG(QLLog, Warning, TEXT("Fire"));
}

void AQLFirewall::StopDamage(AActor* Target)
{
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Target);
	if (Player)
	{
		UAbilitySystemComponent* TargetASC = Cast<UAbilitySystemComponent>(Player->GetAbilitySystemComponent());

		TargetASC->RemoveActiveGameplayEffectBySourceEffect(DotDamageGameplayEffectClass,TargetASC);
	}


	QL_LOG(QLLog, Warning, TEXT("Stop"));
}
