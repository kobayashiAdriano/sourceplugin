#include "Graph/ACFRerouteNode.h"
#include "Graph/ACFComboGraph.h"

UACFRerouteNode::UACFRerouteNode()
{
    NodeName = "Reroute Node";
    NodeParagraph = "...";

#if WITH_EDITOR
    // Assign a custom background color for the editor
    BackgroundColor = FLinearColor(0.2f, 0.6f, 1.0f, 1.0f); // Blue
#endif
}

void UACFRerouteNode::ActivateNode()
{
    UE_LOG(LogTemp, Log, TEXT("ACFRerouteNode::ActivateNode - Activated [%s]."), *NodeName);

    // If this is a proxy node, forward activation to the target reroute node
    if (bIsProxyNode && TargetRerouteNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("ACFRerouteNode::ActivateNode - Forwarding to TargetRerouteNode [%s]."), *TargetRerouteNode->NodeName);

        // Check if the target node is valid and part of the same graph
        if (TargetRerouteNode->GetOuter() == GetOuter())
        {
            UACFComboGraph* CurrentGraph = Cast<UACFComboGraph>(Graph);
            if (CurrentGraph)
            {
                CurrentGraph->InputReceived(CurrentGraph->GetLastInput());
                TargetRerouteNode->ActivateNode();
                return;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ACFRerouteNode::ActivateNode - TargetRerouteNode [%s] is not part of the same graph."), *TargetRerouteNode->NodeName);
        }
    }

    // Perform default behavior for non-proxy nodes
    Super::ActivateNode();

    if (Graph)
    {
        UACFComboGraph* OwningGraph = Cast<UACFComboGraph>(Graph);
        if (OwningGraph)
        {
            bool bTransitioned = OwningGraph->PerformPendingTransition(OwningGraph->GetCharacterOwner());
            if (!bTransitioned)
            {
                UE_LOG(LogTemp, Warning, TEXT("ACFRerouteNode::ActivateNode - No valid transition from [%s]."), *NodeName);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("ACFRerouteNode::ActivateNode - Transition succeeded from [%s]."), *NodeName);
            }
        }
    }
}

#if WITH_EDITOR

FText UACFRerouteNode::GetNodeTitle() const
{
    // If this is a proxy node, show the reference target in parentheses
    if (TargetRerouteNode)
    {
        return FText::Format(FText::FromString("{0} (Proxy to: {1})"),
                             FText::FromString(NodeName.IsEmpty() ? TEXT("Reroute Node") : NodeName),
                             FText::FromString(TargetRerouteNode->NodeName.IsEmpty() ? TEXT("Reroute Node") : TargetRerouteNode->NodeName));
    }

    // Otherwise, just show the node name
    return FText::FromString(NodeName.IsEmpty() ? TEXT("Reroute Node") : NodeName);
}

FText UACFRerouteNode::GetParagraphTitle() const
{
    return FText::FromString(NodeParagraph.IsEmpty() ? TEXT("No additional details.") : NodeParagraph);
}

void UACFRerouteNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UACFRerouteNode, bIsProxyNode))
    {
        // Clear TargetRerouteNode if this is no longer a proxy node
        if (!bIsProxyNode)
        {
            TargetRerouteNode = nullptr;
        }
    }
}

#endif
