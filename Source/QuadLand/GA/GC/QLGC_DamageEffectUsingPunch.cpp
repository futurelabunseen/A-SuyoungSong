// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_DamageEffectUsingPunch.h"
#include "Kismet/GameplayStatics.h"
#include "Character/QLCharacterBase.h"
#include "Sound/SoundWave.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"

UQLGC_DamageEffectUsingPunch::UQLGC_DamageEffectUsingPunch()
{
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundRef(TEXT("/Script/Engine.SoundWave'/Game/Movies/fist-punch-or-kick-7171__mp3cut_net_.fist-punch-or-kick-7171__mp3cut_net_'"));
	if (SoundRef.Object)
	{
		Sound = SoundRef.Object;
	}
}

bool UQLGC_DamageEffectUsingPunch::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{

	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();

	if (HitResult)
	{
		AQLCharacterBase* Character = Cast<AQLCharacterBase>(HitResult->GetActor());
		UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
		if (ASC)
		{
			FGameplayCueParameters CueParams;
			CueParams.EffectContext = Parameters.EffectContext; //���� Context���� ����, HitTarget�� ���� Context
			CueParams.RawMagnitude = 10.f; //���� ������

			ASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_DAMAGESCORE, CueParams); //������ ���ھ ���
		}

	}

	if (Parameters.EffectContext.GetHitResult())
	{
		UGameplayStatics::SpawnSoundAtLocation(Target, Sound, Parameters.Location, FRotator::ZeroRotator);
	}
	
	return false;
}
