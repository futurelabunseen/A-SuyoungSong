
#pragma once

UENUM()
enum class ECharacterAttackType : uint8
{
	HookAttack,
	GunAttack,
	AutomaticGunAttack,
	BombAttack,
	None
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	TypeA,
	TypeB,
	TypeC,
	SpecialTypeA,
	SpecialTypeB,
	None, //None은 Punch를 의미함.
};

