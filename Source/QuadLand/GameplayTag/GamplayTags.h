#pragma once

#include "GameplayTagContainer.h"

#define CHARACTER_EQUIP_NON FGameplayTag::RequestGameplayTag(FName("Character.Equip.NonEquip"))
#define CHARACTER_EQUIP_GUNTYPEA FGameplayTag::RequestGameplayTag(FName("Character.Equip.GunTypeA"))

#define CHARACTER_ATTACK_HITCHECK FGameplayTag::RequestGameplayTag(FName("Character.Attack.HitCheck")) //Punch - Gun �±׿� ���� ���� 
#define CHARACTER_ATTACK_TAKENDAMAGE FGameplayTag::RequestGameplayTag(FName("Character.Attack.TakenDamage")) //Damage�� �޾�����

#define GAMEPLAYCUE_CHARACTER_FIREEFFECT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.FireEffect"))
#define GAMEPLAYCUE_CHARACTER_DAMAGEEFFECT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.DamageEffect"))

//Data �ʱ�ȭ �±�
#define DATA_STAT_AMMOCNT FGameplayTag::RequestGameplayTag(FName("Stat.Data.AmmoCnt"))
#define DATA_STAT_DAMAGE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Demage"))
#define DATA_INIT_ATTACKDISTANCE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Distance"))


#define CHARACTER_STATE_WIN FGameplayTag::RequestGameplayTag(FName("Character.State.Win"))
#define CHARACTER_STATE_DEAD FGameplayTag::RequestGameplayTag(FName("Character.State.Dead"))
