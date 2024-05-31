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
		//������ Muzzle �Ա�
		const FVector &TargetPos = Parameters.Location;
		const FVector &OriginalPos = Character->GetWeaponMuzzlePos();


		//Projecttile �Ἥ �������� ����ϴ� ���ۿ� ������.. OriginalPos ����
		//TargetPos �� ��� 
	
#if ENABLE_DRAW_DEBUG
		DrawDebugLine(GetWorld(), OriginalPos, TargetPos, FColor::Red, false, 5.0f);
#endif
	}

	//���� Target ��ġ 
	return false;
}
