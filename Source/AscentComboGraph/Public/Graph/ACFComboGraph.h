// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#pragma once

#include "AGSGraph.h"
#include "ARSTypes.h"
#include "CoreMinimal.h"
#include "ACFBaseComboNode.h"

#include <GameFramework/Character.h>
#include <GameplayTagContainer.h>

#include "ACFComboGraph.generated.h"

class UInputAction;
class UACFComboNode;
class UACFBaseAction;
class UACFTransition;


/**
 *
 */
UENUM()
enum class EComboState : uint8 {
    /** Node is enabled. */
    Started,
    /** Node is disabled. */
    NotStarted,

};

UCLASS()
class ASCENTCOMBOGRAPH_API UACFComboGraph : public UAGSGraph {
    GENERATED_BODY()

private:
    EComboState Enabled = EComboState::NotStarted;

    TObjectPtr<ACharacter> characterOwner;

    FGameplayTag triggeringAction;

    TObjectPtr<UInputAction> storedInput;

    bool PerformTransition(UInputAction* currentInput, const ACharacter* Character);

protected:
    virtual bool ActivateNode(class UAGSGraphNode* node) override;
    // New Methods
    bool HandleRerouteNode(UACFBaseComboNode* comboNode);
    TArray<TPair<UAGSGraphNode*, UACFTransition*>> GetSortedTransitions(UACFBaseComboNode* comboNode);
    bool HandleAITransitions(const TArray<TPair<UAGSGraphNode*, UACFTransition*>>& Transitions, const ACharacter* Character, UACFBaseComboNode* CurrentNode);
    bool HandlePlayerTransitions(const TArray<TPair<UAGSGraphNode*, UACFTransition*>>& Transitions, UInputAction* currentInput, const ACharacter* Character, UACFBaseComboNode* CurrentNode);
    // AI-specific state
    UPROPERTY()
    bool bIsAI;

public:
    UACFComboGraph();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StartCombo(const FGameplayTag& inStartAction);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StopCombo();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void InputReceived(UInputAction* currentInput);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool PerformPendingTransition(const ACharacter* Character);
  

    UFUNCTION(BlueprintPure, Category = ACF)
    UAnimMontage* GetCurrentComboMontage() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool GetCurrentComboModifier(FAttributesSetModifier& outModifier) const;

    // AI/Player State Management
    void SetIsAI(bool bInIsAI);
    bool IsAI() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsActive() const
    {
        return Enabled == EComboState::Started;
    }

    class ACharacter* GetCharacterOwner() const
    {
        return characterOwner;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetTriggeringAction() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    UACFComboNode* GetCurrentComboNode() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    UInputAction* GetLastInput() const
    {
        return storedInput;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool HasPendingInput() const
    {
        return storedInput != nullptr;
    }

    // They can be different instances, but they are the same combo
    FORCEINLINE bool operator==(const UACFComboGraph* Other) const { return this->GetClass() == Other->GetClass(); }

    FORCEINLINE bool operator!=(const UACFComboGraph* Other) const { return this->GetClass() != Other->GetClass(); }

    UWorld* GetWorld() const override { return characterOwner ? characterOwner->GetWorld() : nullptr; }

    
    TArray<UInputAction*> GetValidInputsForCurrentNode() const;
};
