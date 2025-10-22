// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ACFComboComponent.generated.h"


class UEnhancedInputComponent;
class UEnhancedPlayerInput;
class UACFComboGraph;
class UInputAction;
class UACFActionsManagerComponent;

UCLASS(ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOMBOGRAPH_API UACFComboComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFComboComponent();

    UFUNCTION(BlueprintCallable,  Server, Reliable, Category = ACF)
    void StartCombo(UACFComboGraph* comboToStart, const FGameplayTag& triggeringAction);

    UFUNCTION(BlueprintCallable,  Server, Reliable, Category = ACF)
    void StopCombo(UACFComboGraph* comboToStart);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void SetInputBufferOpened(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsExecutingCombo(UACFComboGraph* combo) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsExecutingAnyCombo() const;

    UPROPERTY(Replicated)
    bool bIsPerformingCombo;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

    UPROPERTY(EditAnywhere, Category = ACF)
    TArray<UInputAction*> ComboInputs;
    

private:
    UFUNCTION()
    void HandleComboInputReceived(UInputAction* input);

    UFUNCTION(Server, Reliable)
    void SendInputReceived(UInputAction* input);

    void StopCurrentCombo();

    UPROPERTY(Replicated)
    bool bIsInputBufferEnabled;
    
    TObjectPtr<UACFComboGraph> currentCombo;
    TObjectPtr<UACFActionsManagerComponent> actionsComp;
};
