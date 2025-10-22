// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#pragma once

#include "CineCameraActor.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "Graph/ADSDialogue.h"
#include <Components/SceneComponent.h>
#include <GameplayTagContainer.h>

#include "ADSDialoguePartecipantComponent.generated.h"

class USkeletalMeshComponent;
class UADSDialogue;

UCLASS(Blueprintable, ClassGroup = (ATS), meta = (BlueprintSpawnableComponent))
class ASCENTDIALOGUESYSTEM_API UADSDialoguePartecipantComponent : public USceneComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UADSDialoguePartecipantComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void EndPlay(EEndPlayReason::Type reason) override;

public:
    // Attempts to start a dialogue given an array of dialogue participant components and a dialogue object
    UFUNCTION(BlueprintCallable, Category = ADS)
    bool TryStartDialogue(const TArray<UADSDialoguePartecipantComponent*>& participants, UADSDialogue* dialogueToStart);

    // Attempts to start a dialogue using actors instead of participant components
    UFUNCTION(BlueprintCallable, Category = ADS)
    bool TryStartDialogueFromActors(const TArray<AActor*>& participants, UADSDialogue* dialogueToStart);

    // Event triggered when a dialogue starts
    UPROPERTY(BlueprintAssignable, Category = ADS)
    FOnDialogueStarted OnDialogueStarted;

    // Event triggered when a dialogue ends
    UPROPERTY(BlueprintAssignable, Category = ADS)
    FOnDialogueEnded OnDialogueEnded;

    // Returns the name of the participant
    UFUNCTION(BlueprintPure, Category = ADS)
    FORCEINLINE FText GetParticipantName() const
    {
        return ParticipantNameText;
    }

    // Returns the tag associated with the participant
    UFUNCTION(BlueprintPure, Category = ADS)
    FORCEINLINE FGameplayTag GetParticipantTag() const
    {
        return PartecipantTag;
    }

    // Returns the icon associated with the participant
    UFUNCTION(BlueprintPure, Category = ADS)
    FORCEINLINE UTexture2D* GetParticipantIcon() const
    {
        return PartecipantIcon;
    }

    // Returns the socket name where the participant's voice is spawned
    UFUNCTION(BlueprintPure, Category = ADS)
    FORCEINLINE FName GetVoiceSpawningSocketName() const
    {
        return VoiceSpawningSocketName;
    }

    // Retrieves a dialogue by tag, setting bFound to true if found
    UFUNCTION(BlueprintPure, Category = ADS)
    UADSDialogue* GetDialogue(FGameplayTag dialogueTag, bool& bFound) const;

    // Retrieves the skeletal mesh component of the owner
    UFUNCTION(BlueprintPure, Category = ADS)
    USkeletalMeshComponent* GetOwnerMesh();

    // Retrieves the skeletal mesh component used for facial animations
    UFUNCTION(BlueprintPure, Category = ADS)
    USkeletalMeshComponent* GetFacialAnimationMesh();

    // Returns the default camera actor as a hard reference
    UFUNCTION(BlueprintPure, Category = "ADS|Camera")
    ACineCameraActor* GetDefaultCameraActor() const
    {
        return DefaultCameraActor.Get();
    }

    // Getter for DefaultEnforcedPlayerPosition that converts soft reference to hard reference
    UFUNCTION(BlueprintPure, Category = "ADS|Camera")
    ATargetPoint* GetDefaultEnforcedPlayerPosition() const
    {
        return DefaultEnforcedPlayerPosition.Get();
    }

    // Changes the participant's icon
    UFUNCTION(BlueprintCallable, Category = ADS)
    void ChangeParticipantIcon(class UTexture2D* icon)
    {
        PartecipantIcon = icon;
    }

    // Changes the participant's name
    UFUNCTION(BlueprintCallable, Category = ADS)
    void ChangeParticipantName_DEPRECATED(const FName& newName)
    {
        PartecipantName = newName;
    }

     // Changes the participant's name
    UFUNCTION(BlueprintCallable, Category = ADS)
    void SetParticipantName(const FText& newName)
    {
        ParticipantNameText = newName;
    }

    // Sets the skeletal mesh for the participant
    UFUNCTION(BlueprintCallable, Category = ADS)
    void SetParticipantSkeletalMesh(class USkeletalMeshComponent* mesh)
    {
        skeletalMesh = mesh;
    }

    // Plays an animation montage on the character that owns this participant
    UFUNCTION(BlueprintCallable, Category = ADS)
    virtual void PlayAnimationOnCharacterOwner(UAnimMontage* animationTag);

    // Plays a facial animation montage on the character that owns this participant
    UFUNCTION(BlueprintCallable, Category = ADS)
    virtual void PlayFacialAnimationOnCharacterOwner(UAnimMontage* animationTag);

protected:
    /*List of dialogues that can be started for this participant*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    TArray<class UADSDialogue*> Dialogues;

   /** The  unique name identifying this participant. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    FText ParticipantNameText;

    /** The DECPRECATED unique name identifying this participant. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "DEPRECATED Participant Name"),  Category = ADS)
    FName PartecipantName;

    /** Gameplay tag used to group or identify this participant in dialogues. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    FGameplayTag PartecipantTag;

    /** Icon representing this participant in dialogue UIs. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    class UTexture2D* PartecipantIcon;

    /** Socket name for voice or effect spawning during dialogues (e.g., 'head'). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    FName VoiceSpawningSocketName = "head";

    /** Tag for identifying the facial skeleton component. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ADS)
    FName FacialSkeletonComponentTag = "head";

    /** Event called when a dialogue starts. */
    UFUNCTION(BlueprintNativeEvent, Category = ADS)
    void OnDialogueStartedEvent();

    /** Event called when a dialogue ends. */
    UFUNCTION(BlueprintNativeEvent, Category = ADS)
    void OnDialogueEndedEvent();

    /*If not specified in the dialogue start node, this position will be used to place the Player in a conversation
    with this character*/
    UPROPERTY(EditAnywhere, Category = "ADS|Camera")
    TSoftObjectPtr<ATargetPoint> DefaultEnforcedPlayerPosition;

    // If not specified int he dialogue node,
    // The camera used to frame the speaking character during dialogue if
    UPROPERTY(EditAnywhere, Category = "ADS|Camera")
    TSoftObjectPtr<ACineCameraActor> DefaultCameraActor;

    /** Skeletal mesh for the participant. */
    TObjectPtr<USkeletalMeshComponent> skeletalMesh;

    /** Facial skeletal mesh for the participant. */
    TObjectPtr<USkeletalMeshComponent> facialMesh;

private:
    UFUNCTION()
    void HandleDialogueStarted();

    UFUNCTION()
    void HandleDialogueEnded();

    UPROPERTY()
    TArray<TObjectPtr<class UADSDialogue>> instancedDialogues;
};
