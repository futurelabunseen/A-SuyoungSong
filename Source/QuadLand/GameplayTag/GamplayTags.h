#pragma once

#include "GameplayTagContainer.h"

#define CHARACTER_EQUIP_NON FGameplayTag::RequestGameplayTag(FName("Character.Equip.NonEquip"))
#define CHARACTER_EQUIP_GUNTYPEA FGameplayTag::RequestGameplayTag(FName("Character.Equip.GunTypeA"))
#define CHARACTER_EQUIP_BOMB FGameplayTag::RequestGameplayTag(FName("Character.Equip.Bomb"))

#define CHARACTER_ATTACK_HITCHECK FGameplayTag::RequestGameplayTag(FName("Character.Attack.HitCheck")) //Punch - Gun 태그에 따라서 실행 
#define CHARACTER_ATTACK_TAKENDAMAGE FGameplayTag::RequestGameplayTag(FName("Character.Attack.TakenDamage")) //Damage를 받았을때

#define GAMEPLAYCUE_CHARACTER_FIREEFFECT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.FireEffect"))
#define GAMEPLAYCUE_CHARACTER_DAMAGEEFFECT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.DamageEffect"))

//Data 초기화 태그
#define DATA_STAT_AMMOCNT FGameplayTag::RequestGameplayTag(FName("Stat.Data.AmmoCnt"))
#define DATA_STAT_DAMAGE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Demage"))
#define DATA_INIT_ATTACKDISTANCE FGameplayTag::RequestGameplayTag(FName("Stat.Data.Distance"))

#define CHARACTER_STATE_RUN FGameplayTag::RequestGameplayTag(FName("Character.State.Run"))
#define CHARACTER_STATE_STOP FGameplayTag::RequestGameplayTag(FName("Character.State.Stop"))
#define CHARACTER_STATE_NOTRUN  FGameplayTag::RequestGameplayTag(FName("Character.State.NotRun"))
#define CHARACTER_STATE_WIN FGameplayTag::RequestGameplayTag(FName("Character.State.Win"))
#define CHARACTER_STATE_DEAD FGameplayTag::RequestGameplayTag(FName("Character.State.Dead"))
#define CHARACTER_STATE_RELOAD FGameplayTag::RequestGameplayTag(FName("Character.State.Reload")) //Damage를 받았을때
#define CHARACTER_STATE_DANGER FGameplayTag::RequestGameplayTag(FName("Character.State.Danger")) //Damage를 받았을때

#define CHARACTER_STATE_PRONE FGameplayTag::RequestGameplayTag(FName("Character.State.Prone")) //Damage를 받았을때

#define WEAPON_GUN_AUTO FGameplayTag::RequestGameplayTag(FName("Weapon.Gun.Auto")) //Damage를 받았을때
#define WEAPON_GUN_SEMIAUTO FGameplayTag::RequestGameplayTag(FName("Weapon.Gun.Semiauto")) //Damage를 받았을때

#define STAT_STAMINA_STATUS_REDUCE FGameplayTag::RequestGameplayTag(FName("Stat.Stamina.Status.Reduce"))

#define GAMEPLAYCUE_EFFECT_FIREWALL FGameplayTag::RequestGameplayTag(FName("GameplayCue.Effect.Firewall"))