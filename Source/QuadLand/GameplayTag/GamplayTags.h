#pragma once

#include "GameplayTagContainer.h"

#define CHARACTER_EQUIP_NON FGameplayTag::RequestGameplayTag(FName("Character.Equip.NonEquip"))
#define CHARACTER_EQUIP_GUNTYPEA FGameplayTag::RequestGameplayTag(FName("Character.Equip.GunTypeA"))

#define CHARACTER_ATTACK_HITCHECK FGameplayTag::RequestGameplayTag(FName("Character.Attack.HitCheck"))
#define GAMEPLAYCUE_CHARACTER_FIREEFFECT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.FireEffect"))

//Data 초기화 태그
#define DATA_INIT_AMMOCNT FGameplayTag::RequestGameplayTag(FName("Stat.Data.AmmoCnt"))
#define DATA_INIT_DAMAGE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Demage"))
#define DATA_INIT_ATTACKDISTANCE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Distance"))
