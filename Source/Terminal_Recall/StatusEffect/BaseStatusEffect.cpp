// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatusEffect.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"

ABaseStatusEffect::ABaseStatusEffect()
{
}

void ABaseStatusEffect::DoStatusEffect_Implementation(ABattleEntity* entityTarget)
{
	DoStatusEffect(entityTarget);
	Duration--;
	if (Duration <= 0)
	{
		OnEffectEnded();
		Destroy();
	}
}