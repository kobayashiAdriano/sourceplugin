// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "Graph/ACFComboGraph.h"

#include "ACFComboComponent.h"
#include "Actions/ACFBaseAction.h"
#include "Engine/Engine.h"
#include "Graph/ACFComboNode.h"
#include "Graph/ACFRerouteNode.h"
#include "Graph/ACFStartComboNode.h"
#include "Graph/ACFTransition.h"
#include "Graph/ACFBaseComboNode.h"


bool UACFComboGraph::ActivateNode(class UAGSGraphNode* node)
{
    return Super::ActivateNode(node);
}

UACFComboGraph::UACFComboGraph()
{
    NodeType = UACFBaseComboNode::StaticClass();
    EdgeType = UACFTransition::StaticClass();
    Enabled = EComboState::NotStarted;
}

void UACFComboGraph::StartCombo(const FGameplayTag& inStartAction)
{
    DeactivateAllNodes();
    storedInput = nullptr; // Clear previous input
    for (UAGSGraphNode* root : RootNodes) {
        UACFStartComboNode* startNode = Cast<UACFStartComboNode>(root);
        if (startNode && startNode->GetTriggeringAction() == inStartAction) {  
            Enabled = EComboState::Started;
            ActivateNode(startNode);
            return;
        }
    }
}

void UACFComboGraph::StopCombo()
{
    DeactivateAllNodes();
    Enabled = EComboState::NotStarted;
}

void UACFComboGraph::InputReceived(UInputAction* currentInput)
{
    storedInput = currentInput;

}

bool UACFComboGraph::PerformPendingTransition(const ACharacter* Character)
{

    if (Character)
    {
        characterOwner = const_cast<ACharacter*>(Character);
    }
    if (PerformTransition(GetLastInput(), characterOwner)) {
        storedInput = nullptr;
        return true;
    }
    return false;
   
}

bool UACFComboGraph::PerformTransition(UInputAction* currentInput, const ACharacter* Character)
{
    for (auto node : GetActiveNodes())
    {
        UACFBaseComboNode* comboNode = Cast<UACFBaseComboNode>(node);
        if (comboNode)
        {
            if (HandleRerouteNode(comboNode))
            {
                return true;
            }

            // Collect and sort edges
            TArray<TPair<UAGSGraphNode*, UACFTransition*>> SortedTransitions = GetSortedTransitions(comboNode);
            // Check if AI and delegate logic
            if (IsAI())
            {
                if (HandleAITransitions(SortedTransitions, Character, comboNode))
                {
                    return true;
                }
            }
            else
            {
                if (HandlePlayerTransitions(SortedTransitions, currentInput, Character, comboNode))
                {
                    return true;
                }
            }
        }
    }
    StopCombo(); // Gracefully terminate the combo if no valid transitions are found
    return false; // No valid transition found
}

bool UACFComboGraph::HandleRerouteNode(UACFBaseComboNode* comboNode)
{
    UACFRerouteNode* rerouteNode = Cast<UACFRerouteNode>(comboNode);
    if (rerouteNode) {
        if (rerouteNode->TargetRerouteNode && rerouteNode->bIsProxyNode)
        {
            DeactivateNode(rerouteNode);
            ActivateNode(rerouteNode->TargetRerouteNode);
            return true;
        }
    }
   
    return false;
}

TArray<TPair<UAGSGraphNode*, UACFTransition*>> UACFComboGraph::GetSortedTransitions(UACFBaseComboNode* comboNode)
{
    TArray<TPair<UAGSGraphNode*, UACFTransition*>> SortedTransitions;

    for (auto edge : comboNode->Edges)
    {
        UAGSGraphNode* TargetNode = Cast<UAGSGraphNode>(edge.Key);
        UACFTransition* Transition = Cast<UACFTransition>(edge.Value);

        if (TargetNode && Transition)
        {
            SortedTransitions.Add(TPair<UAGSGraphNode*, UACFTransition*>(TargetNode, Transition));
        }
    }

    SortedTransitions.Sort([](const TPair<UAGSGraphNode*, UACFTransition*>& A, const TPair<UAGSGraphNode*, UACFTransition*>& B) {
        if (A.Value->bUseWeightedPriorities && B.Value->bUseWeightedPriorities)
        {
            float TotalWeight = A.Value->Weight + B.Value->Weight;
            float ProbabilityA = A.Value->Weight / TotalWeight;
            return FMath::FRand() < ProbabilityA;
        }
        else if (A.Value->bUseWeightedPriorities)
        {
            return false;
        }
        else if (B.Value->bUseWeightedPriorities)
        {
            return true;
        }
        return A.Value->Priority > B.Value->Priority;
    });


    // Filter sorted transitions by valid pair.key
    SortedTransitions = SortedTransitions.FilterByPredicate([](const TPair<UAGSGraphNode*, UACFTransition*>& Pair) {
            return Pair.Key != nullptr;
    });

    return SortedTransitions;
}

bool UACFComboGraph::HandleAITransitions(
    const TArray<TPair<UAGSGraphNode*, UACFTransition*>>& Transitions,
    const ACharacter* Character,
    UACFBaseComboNode* CurrentNode)
{
    // Get the simulated input from the current combo
    UInputAction* SimulatedInput = GetLastInput();
    if (!SimulatedInput)
    {
        return false;
    }

    // Evaluate transitions based on the simulated input
    for (const auto& Pair : Transitions)
    {
        UAGSGraphNode* TargetNode = Pair.Key;
        UACFTransition* Transition = Pair.Value;

        if (Transition->GetTransitionInput() == SimulatedInput && Transition->AreConditionsMet(Character))
        {
            UACFBaseComboNode* newNode = Cast<UACFBaseComboNode>(TargetNode);
            ensure(newNode);

            DeactivateNode(CurrentNode);
            ActivateNode(newNode);

            return true;
        }
    }

    return false; // No valid transition found
}

void UACFComboGraph::SetIsAI(bool bInIsAI)
{
    bIsAI = bInIsAI;
}

bool UACFComboGraph::IsAI() const
{
    return bIsAI;
}

TArray<UInputAction*> UACFComboGraph::GetValidInputsForCurrentNode() const
{
    TArray<UInputAction*> ValidInputs;

    for (UAGSGraphNode* Node : GetActiveNodes())
    {
        UACFBaseComboNode* ComboNode = Cast<UACFBaseComboNode>(Node);
        if (ComboNode)
        {
            for (const auto& Edge : ComboNode->Edges)
            {
                UACFTransition* Transition = Cast<UACFTransition>(Edge.Value);
                if (Transition && Transition->GetTransitionInput())
                {
                    ValidInputs.AddUnique(Transition->GetTransitionInput());
                }
            }
        }
    }

    return ValidInputs;
}


bool UACFComboGraph::HandlePlayerTransitions(
    const TArray<TPair<UAGSGraphNode*, UACFTransition*>>& Transitions,
    UInputAction* currentInput,
    const ACharacter* Character,
    UACFBaseComboNode* CurrentNode)
{
    for (const auto& Pair : Transitions)
    {
        UAGSGraphNode* TargetNode = Pair.Key;
        UACFTransition* Transition = Pair.Value;

        if (Transition->GetTransitionInput() == currentInput && Transition->AreConditionsMet(Character))
        {
            UACFBaseComboNode* newNode = Cast<UACFBaseComboNode>(TargetNode);
            ensure(newNode);

            DeactivateNode(CurrentNode);
            ActivateNode(newNode);

            return true;
        }
    }
    return false;
}

FGameplayTag UACFComboGraph::GetTriggeringAction() const
{
    return triggeringAction;
}

UACFComboNode* UACFComboGraph::GetCurrentComboNode() const
{
    if (IsActive() && GetActiveNodes().IsValidIndex(0) && IsValid(GetActiveNodes()[0])) {
        // only one combo anim can be reproduced at time
        return Cast<UACFComboNode>(GetActiveNodes()[0]);
    }
    return nullptr;
}

UAnimMontage* UACFComboGraph::GetCurrentComboMontage() const
{
    UACFComboNode* currentNode = GetCurrentComboNode();
    if (currentNode) {
        return currentNode->GetMontage();
    }
    return nullptr;
}

bool UACFComboGraph::GetCurrentComboModifier(FAttributesSetModifier& outModifier) const
{
    UACFComboNode* currentNode = GetCurrentComboNode();
    if (currentNode) {
        outModifier = currentNode->GetComboNodeModifier();
        return true;
    }
    return false;
}


