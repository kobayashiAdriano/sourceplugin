// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "ANSUIPlayerSubsystem.h"
#include "ANSDeveloperSettings.h"
#include "ANSUITypes.h"
#include "Blueprint/UserWidget.h"
#include "CommonActionWidget.h"
#include "Engine/DataTable.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Input/CommonUIInputSettings.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "UITag.h"
#include <CommonButtonBase.h>

UUserWidget* UANSUIPlayerSubsystem::SpawnInGameWidget(TSubclassOf<UUserWidget> widgetClass, bool bPauseGame /*= true*/, bool bLockGameInput)
{
    APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

 
    UUserWidget* spawnedWidget = CreateWidget<UUserWidget>(playerController, widgetClass);
    if (spawnedWidget && playerController) {
        currentWidget = spawnedWidget;
        spawnedWidget->AddToViewport();
        bDefaultPauseGame = bPauseGame;

        if (bLockGameInput) {
            FInputModeUIOnly NewInputMode;
            playerController->SetInputMode(NewInputMode);
            NewInputMode.SetWidgetToFocus(spawnedWidget->GetCachedWidget());
        } else {
            FInputModeGameAndUI NewInputMode;
            playerController->SetInputMode(NewInputMode);
            NewInputMode.SetWidgetToFocus(spawnedWidget->GetCachedWidget());
        }

        // NewInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

        UGameplayStatics::SetGamePaused(playerController, bPauseGame);

        APawn* pawn = playerController->GetPawn();
        if (pawn && pawn->GetMovementComponent()) {
            pawn->GetMovementComponent()->StopMovementImmediately();
        }
        if (widgetStack.Contains(widgetClass)) {
            widgetStack.Remove(widgetClass);
        }
        widgetStack.Add(widgetClass);
     
    }
    return spawnedWidget;
}

void UANSUIPlayerSubsystem::RemoveInGameWidget(UUserWidget* widget,  bool bUnlockUIInput, bool bRemovePause)
{
    APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

    if (widget && playerController) {
        if (bRemovePause) {
            UGameplayStatics::SetGamePaused(playerController, false);
        }

        if (bUnlockUIInput) {
            FInputModeGameOnly NewInputMode;
            playerController->SetInputMode(NewInputMode);
        } else {
            FInputModeUIOnly NewInputMode;
            playerController->SetInputMode(NewInputMode);
        }

        widget->RemoveFromParent();
    }
}

void UANSUIPlayerSubsystem::GoToPreviousWidget()
{
    TSubclassOf<UUserWidget> previousWidget = widgetStack.Last();

    if (currentWidget && currentWidget->GetClass() == previousWidget) {
        RemoveInGameWidget(currentWidget, true, false);
        const int32 lastIndex = widgetStack.Num() - 1;
        widgetStack.RemoveAt(lastIndex);
    }

    if (widgetStack.Num() > 0) {
        previousWidget = widgetStack.Last();
        if (previousWidget) {
            widgetStack.Remove(previousWidget);
            SpawnInGameWidget(previousWidget, UGameplayStatics::IsGamePaused(this));
        }
    }
}

bool UANSUIPlayerSubsystem::TryGetActionsFromKey(const FKey& key, TArray<FUIActionTag>& outActionsTag)
{
    const UCommonUIInputSettings* inputSett = GetInputSettings();
    const TArray<FUIInputAction> actionsList = inputSett->GetUIInputActions();
    bool bFound = false;
    for (const FUIInputAction& action : actionsList) {
        for (const FUIActionKeyMapping& mapping : action.KeyMappings) {
            if (mapping.Key == key) {
                outActionsTag.Add(action.ActionTag);
                bFound = true;
            }
        }
    }
    return bFound;
}

bool UANSUIPlayerSubsystem::TryGetKeysForAction(const FUIActionTag& UIAction, TArray<FKey>& outKeys)
{
    const UCommonUIInputSettings* inputSett = GetInputSettings();
    const TArray<FUIInputAction> actionsList = inputSett->GetUIInputActions();
    bool bFound = false;
    for (const FUIInputAction& action : actionsList) {

        if (action.ActionTag == UIAction) {
            outKeys.Empty();
            for (const auto& keyMap : action.KeyMappings) {
                outKeys.Add(keyMap.Key);
            }       
            bFound = true;
        }
    }
    return bFound;
}

UCommonUIInputSettings* UANSUIPlayerSubsystem::GetInputSettings() const
{
    return GetMutableDefault<UCommonUIInputSettings>();
}

UANSDeveloperSettings* UANSUIPlayerSubsystem::GetUISettings() const
{
    return GetMutableDefault<UANSDeveloperSettings>();
}

UTexture2D* UANSUIPlayerSubsystem::GetIconByTag(FGameplayTag iconTag)
{
    const UANSDeveloperSettings* UISetting = GetUISettings();

    const UDataTable* IconsByTag = UISetting->GetIconsByTagDT();
    if (!IconsByTag) {
        UE_LOG(LogTemp, Error, TEXT("Remember to set your UI Icons By Tag! - UANSUIPlayerSubsystem::GetIconByTag"));
        return nullptr;
    }

    for (const auto& row : IconsByTag->GetRowMap()) {
        FANSIcons* icon = (FANSIcons*)(row.Value);

        if (!icon) {
            UE_LOG(LogTemp, Error, TEXT("Wrong DB Type! - UANSUIPlayerSubsystem::GetIconByTag "));
            return nullptr;
        }
        if (iconTag == icon->IconTag) {
            return icon->Icon;
        }
    }

    return nullptr;
}

bool UANSUIPlayerSubsystem::TryGetActionConfig(FUIActionTag actionName, const ECommonInputType& inputType, FANSActionConfig& outAction)
{
    const UCommonUIInputSettings* inputSettings = GetInputSettings();

    const TArray<FUIInputAction> actionsList = inputSettings->GetUIInputActions();
    const FUIInputAction* action = actionsList.FindByPredicate([actionName](const FUIInputAction& Action) { return Action.ActionTag == actionName; });

    if (!action) {
        UE_LOG(LogTemp, Error, TEXT("Remember to set your UI Actions in Common UI Input Settings"));
        return false;
    }

    outAction.Action = actionName;
    outAction.UIName = action->DefaultDisplayName;

    for (const auto& keymap : action->KeyMappings) {
        if (inputType == ECommonInputType::Gamepad && keymap.Key.IsGamepadKey() || inputType == ECommonInputType::MouseAndKeyboard && !keymap.Key.IsGamepadKey()) {
            outAction.KeyIcon = GetCurrentPlatformIconForKey(keymap.Key);
            return true;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Remember to set your Icons! -UANSNavbarComponent::TryGetActionFromKey "));
    return false;
}

class UTexture2D* UANSUIPlayerSubsystem::GetIconForUIAction(FUIActionTag actionName, const ECommonInputType& inputType)
{
    FANSActionConfig outConfig;
    UANSUIPlayerSubsystem::TryGetActionConfig(actionName, inputType, outConfig);
    return outConfig.KeyIcon;
}

class UTexture2D* UANSUIPlayerSubsystem::GetCurrentPlatformIconForKey(const FKey& key)
{
    const FString platform = UGameplayStatics::GetPlatformName();
    return GetIconForKey(key, platform);
}

class UTexture2D* UANSUIPlayerSubsystem::GetIconForKey(const FKey& key, const FString& platform)
{
    const UANSDeveloperSettings* UISetting = GetUISettings();

    const UDataTable* platformIcons = UISetting->GetKeysConfigByPlatformDT(platform);
    if (!platformIcons) {
        UE_LOG(LogTemp, Error, TEXT("Remember to set your UI Icons! -UANSNavbarComponent::GetIconByTag "));
        return nullptr;
    }

    for (const auto& row : platformIcons->GetRowMap()) {
        FANSKeysIconConfig* iconConfig = (FANSKeysIconConfig*)(row.Value);
        if (iconConfig && iconConfig->Key == key) {
            return iconConfig->KeyIcon;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Remember to set your Keys!! -UANSNavbarComponent::GetIconForKey "));
    return nullptr;
}
