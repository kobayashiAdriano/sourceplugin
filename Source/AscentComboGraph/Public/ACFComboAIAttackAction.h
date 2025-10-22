#pragma once

#include "CoreMinimal.h"
#include "ACFComboAttackAction.h"
#include "ACFComboAIAttackAction.generated.h"

// Forward Declarations
class UInputAction;
class UACFComboGraph;
class UACFComboNode;
class UACFComboComponent;

/**
 * A specialized combo attack action for AI characters.
 */
UCLASS()
class ASCENTCOMBOGRAPH_API UACFComboAIAttackAction : public UACFComboAttackAction
{
    GENERATED_BODY()

public:
    UACFComboAIAttackAction();

    // AI-specific input probabilities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TMap<UInputAction*, float> AIInputProbabilities;

protected:
    virtual void OnActionStarted_Implementation(const FString& contextString = TEXT("")) override;
    virtual void OnActionEnded_Implementation() override;

private:
    void SimulateAIInput();
};
