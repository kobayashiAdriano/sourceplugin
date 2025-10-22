// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "ACFComboAttackAction.h"
#include "ACFComboComponent.h"
#include "ARSStatisticsComponent.h"
#include "Graph/ACFComboGraph.h"
#include "Graph/ACFComboNode.h"

void UACFComboAttackAction::OnActionStarted_Implementation(const FString& contextString /*= ""*/)
{
    if (Combo && CharacterOwner) {
        comboComponent = CharacterOwner->FindComponentByClass<UACFComboComponent>();
        if (comboComponent) {
            // comboInst = DuplicateObject<UACFComboGraph>(Combo,CharacterOwner);
            if (!comboComponent->IsExecutingCombo(Combo)) {
                comboComponent->StartCombo(Combo, GetActionTag());
              
            }
            node = Combo->GetCurrentComboNode();

            if (!node || !IsValid(node) || !node->IsValidLowLevel()) {
                UE_LOG(LogTemp, Error, TEXT("Invalid Combo Transition in %s! Node: %s, Combo: %s"), 
                    *GetNameSafe(this),
                    node ? *GetNameSafe(node) : TEXT("nullptr"),
                    *GetNameSafe(Combo));
                
                // First stop the combo to prevent recursion
                if (comboComponent && comboComponent->IsExecutingCombo(Combo)) {
                    comboComponent->StopCombo(Combo);
                }
                
                ExitAction();
                return;
            }
            if (bAutoOpenBuffer) {
                comboComponent->SetInputBufferOpened(true);
            }

            if (node) {
                // FString MontageName = node ? node->GetMontage()->GetName() : "None";
                SetAnimMontage(node->GetMontage());
                DamageToActivate = node->GetDamageToActivate();
                TraceChannels = node->GetTraceChannels();
                UARSStatisticsComponent* statComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
                if (statComp) {
                    statComp->AddAttributeSetModifier(node->GetComboNodeModifier());
                }
            }
        }
    }
    Super::OnActionStarted_Implementation(contextString);
}

void UACFComboAttackAction::OnActionEnded_Implementation()
{
    if (node && comboComponent) {
        UARSStatisticsComponent* statComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
        if (statComp) {
            statComp->RemoveAttributeSetModifier(node->GetComboNodeModifier());
        }
        if (bAutoOpenBuffer) {
            comboComponent->SetInputBufferOpened(false);
        }
        bSuccesfulCombo = Combo->HasPendingInput();
        if (bSuccesfulCombo && Combo->PerformPendingTransition(CharacterOwner))
        {
            ActionsManager->StoreAction(GetActionTag());   
        } else {
            comboComponent->StopCombo(Combo);    
        }
    }
   
    Super::OnActionEnded_Implementation();
}

UACFComboAttackAction::UACFComboAttackAction()
{
    bAutoOpenBuffer = true;
}
