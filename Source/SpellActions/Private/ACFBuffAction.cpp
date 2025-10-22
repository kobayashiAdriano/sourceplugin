// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "ACFBuffAction.h"
#include "ARSStatisticsComponent.h"
#include "Actions/ACFBaseAction.h"
#include "Game/ACFFunctionLibrary.h"

void UACFBuffAction::OnNotablePointReached_Implementation()
{
    Super::OnNotablePointReached_Implementation();
    UARSStatisticsComponent* statComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
    if (statComp) {
        statComp->AddTimedAttributeSetModifier(BuffToApply.modifier, BuffToApply.duration);
        UACFFunctionLibrary::PlayActionEffectLocally(ActionConfig.ActionEffect, CharacterOwner, this);
    }
}
