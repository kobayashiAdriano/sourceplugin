// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graph/ADSGraphNode.h"

#include "ADSDialogueNode.generated.h"

class ACineCameraActor;
class ATargetPoint;

/**
 *
 */
UCLASS()
class ASCENTDIALOGUESYSTEM_API UADSDialogueNode : public UADSGraphNode {
    GENERATED_BODY()

protected:
    // The camera used to frame the speaking character during dialogue
    UPROPERTY(EditAnywhere, Category = "ADS|Camera")
    TSoftObjectPtr<ACineCameraActor> CameraActor;

    // The time taken to blend smoothly between camera transitions during dialogue
    UPROPERTY(EditAnywhere, Category = "ADS|Camera")
    float BlendTime = 0.02f;

    // Conditions that must be met for the dialogue or interaction to be activated
    // These are instances of UAGSCondition and can be customized per instance
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = ADS)
    TArray<class UAGSCondition*> ActivationConditions;

    virtual void ActivateNode() override;

public:
    UADSDialogueNode();

    UFUNCTION(BlueprintCallable, Category = ADS)
    TArray<class UADSDialogueResponseNode*> GetAllValidAnswers(APlayerController* inController);

    virtual bool CanBeActivated(APlayerController* inController) override;
};
