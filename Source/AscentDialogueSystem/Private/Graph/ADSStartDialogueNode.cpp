// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "Graph/ADSStartDialogueNode.h"
#include "ADSDialogueFunctionLibrary.h"
#include "ADSDialogueMasterComponent.h"
#include "ADSDialoguePartecipantComponent.h"
#include "AGSAction.h"
#include "AGSCondition.h"
#include <Components/ActorComponent.h>
#include <Engine/TargetPoint.h>
#include <Kismet/KismetMathLibrary.h>

UADSStartDialogueNode::UADSStartDialogueNode()
{
#if WITH_EDITOR
    BackgroundColor = FLinearColor::Green;
    ContextMenuName = FText::FromString("Start Dialogue Node");
#endif
}

bool UADSStartDialogueNode::CanBeActivated(APlayerController* inController)
{
    return Super::CanBeActivated(inController);
}

void UADSStartDialogueNode::ExecuteEndingActions()
{
    for (UAGSAction* action : DialogueEndedActions) {
        if (action) {
            action->Execute(controller, this);
        }
    }
}

void UADSStartDialogueNode::ActivateNode()
{

    Super::ActivateNode();

    UADSDialoguePartecipantComponent* partComp = GetDialogueParticipant();
    if (partComp) {
        const AActor* dialogueStarter = partComp->GetOwner();
        const ATargetPoint* finalPosition = EnforcedPlayerPosition.Get() ? EnforcedPlayerPosition.Get() : partComp->GetDefaultEnforcedPlayerPosition();
        if (controller && finalPosition && dialogueStarter && controller->GetPawn()) {
            UADSDialogueMasterComponent* dialogueMaster = UADSDialogueFunctionLibrary::GetLocalDialogueMaster(dialogueStarter);
            if (dialogueMaster) {
                const FRotator playerRot = UKismetMathLibrary::FindLookAtRotation(finalPosition->GetActorLocation(), dialogueStarter->GetActorLocation());
                FTransform newTrans(playerRot, finalPosition->GetActorLocation());
                dialogueMaster->MoveControlledPlayerToPosition(newTrans);
            }
        }
        if (partComp && controller && !CameraActor.Get() && partComp->GetDefaultCameraActor() ) {
            controller->SetViewTargetWithBlend(partComp->GetDefaultCameraActor(), BlendTime);
        }
    }
}
