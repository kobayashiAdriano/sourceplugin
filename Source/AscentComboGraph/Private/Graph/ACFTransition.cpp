// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved. 

#include "Graph/ACFTransition.h"
#include "ACFActionCondition.h"
#include "Actors/ACFCharacter.h"


bool UACFTransition::AreConditionsMet(const ACharacter* Character) const
{
    // If no conditions are set, the transition is automatically valid
    UE_LOG(LogTemp, Warning, TEXT("No of Conditions: %d"), Conditions.Num());
    if (Conditions.Num() == 0) {
        return true;
    }
    if (!Character) {
        UE_LOG(LogTemp, Error, TEXT("Character is null!"));
        return false;
    }

    // Attempt to cast ACharacter to AACFCharacter
    const AACFCharacter* ACFCharacter = Cast<AACFCharacter>(Character);
    if (!ACFCharacter) {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast character to AACFCharacter! Transition conditions cannot be evaluated."));
        return false; // Conditions cannot be met without AACFCharacter
    }

    // Log the successful casting
    UE_LOG(LogTemp, Warning, TEXT("Successfully cast character to AACFCharacter: %s"), *ACFCharacter->GetName());

    // Evaluate each condition
    for (const UACFActionCondition* Condition : Conditions) {
        if (!Condition) {
            UE_LOG(LogTemp, Error, TEXT("Condition is null! Skipping."));
            continue;
        }

        // Cast to non-const to call IsConditionMet
        UACFActionCondition* MutableCondition = const_cast<UACFActionCondition*>(Condition);
        const bool bConditionMet = MutableCondition->IsConditionMet(ACFCharacter);

        // Log the result of the condition
        UE_LOG(LogTemp, Warning, TEXT("Condition %s evaluated to: %s"),
            *MutableCondition->GetName(), bConditionMet ? TEXT("True") : TEXT("False"));

        if (!bConditionMet) {
            UE_LOG(LogTemp, Warning, TEXT("Condition %s failed!"), *MutableCondition->GetName());
            return false; // Fail if any condition is not met
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("All conditions passed for transition"));
    return true; // All conditions passed
}

