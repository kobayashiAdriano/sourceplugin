// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "Graph/AQSBaseNode.h"
#include "AGSAction.h"
#include <Kismet/GameplayStatics.h>

void UAQSBaseNode::ActivateNode()
{
    Super::ActivateNode();

    for (UAGSAction* action : ActivationActions) {
        action->Execute(GetPlayerController(), this);
    }
}

void UAQSBaseNode::DeactivateNode()
{
    Super::DeactivateNode();

    for (UAGSAction* action : OnCompletionActions) {
        action->Execute(GetPlayerController(), this);
    }
}

bool UAQSBaseNode::CanBeActivated() const
{
    for (auto parent : ParentNodes) {
        UAQSBaseNode* parentNode = Cast<UAQSBaseNode>(parent);

        if (parentNode && parentNode->IsCompleted()) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

UAQSBaseNode::UAQSBaseNode()
{
#if WITH_EDITOR
    BackgroundColor = FLinearColor::Black;
#endif
}
