// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GC/QLGC_ImpactByGun.h"
#include "Character/QLCharacterPlayer.h"
#include "Item/QLWeaponComponent.h"

#include "Kismet/GameplayStatics.h"
UQLGC_ImpactByGun::UQLGC_ImpactByGun()
{
}

bool UQLGC_ImpactByGun::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(Parameters.Instigator);

	if (Character)
	{
		//시작점 Muzzle 입구
		const FVector &TargetPos = Parameters.Location;
		const FVector &OriginalPos = Character->GetWeaponMuzzlePos();


		//Projecttile 써서 목적지로 출발하는 수밖에 없나봄.. OriginalPos 생성
		//TargetPos 로 출발 
	
#if ENABLE_DRAW_DEBUG
		DrawDebugLine(GetWorld(), OriginalPos, TargetPos, FColor::Red, false, 5.0f);
#endif
	}

	//끝점 Target 위치 
	return false;
}
