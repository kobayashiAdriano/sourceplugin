// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved.

#include "ACFRiderAIController.h"
#include "ACFRiderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <GameFramework/Controller.h>
#include <Camera/PlayerCameraManager.h>
#include <Kismet/GameplayStatics.h>

AACFRiderAIController::AACFRiderAIController()
{
    OffsetRotCorrection = FRotator(0.f, 0.f, 0.f);
    ClampMin = FRotator(-90.f, -90.f, -10.f);
    ClampMax = FRotator(90.f, 90.f, 10.f);
}

void AACFRiderAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn /*= true*/)
{
    if (bOverrideControlWithMountRotation && riderComp && riderComp->IsRiding()) {
        const APawn* mount = riderComp->GetMount();
        if (mount) {
            FRotator mountRot = mount->GetControlRotation();
            mountRot += OffsetRotCorrection;
            const FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(mountRot, GetPawn()->GetActorRotation());
            if (deltaRot.Yaw <= ClampMax.Yaw && deltaRot.Yaw >= ClampMin.Yaw && deltaRot.Pitch <= ClampMax.Pitch && deltaRot.Pitch >= ClampMin.Pitch && deltaRot.Pitch <= ClampMax.Pitch && deltaRot.Pitch >= ClampMin.Pitch) {
                SetControlRotation(mountRot);
            } else {
                Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
            }

        } else {
            Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
        }
    } else {
        Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
    }
}

void AACFRiderAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    SetRiderCompReference();
}

void AACFRiderAIController::OnUnPossess()
{
    Super::OnUnPossess();
    riderComp = nullptr;
}

void AACFRiderAIController::GetPlayerViewPoint(FVector& out_Location, FRotator& out_Rotation) const
{
    APlayerController* pc = Cast<APlayerController>(GetMountController().Get());

    if (pc) {
        pc->GetPlayerViewPoint(out_Location, out_Rotation);
    } else {
        Super::GetPlayerViewPoint(out_Location, out_Rotation);
    }
   
}

void AACFRiderAIController::SetRiderCompReference()
{
    const APawn* possessedPawn = GetPawn();

    ensure(possessedPawn);
    riderComp = possessedPawn->FindComponentByClass<UACFRiderComponent>();
}

TObjectPtr<AController> AACFRiderAIController::GetMountController() const
{
    const APawn* mount = riderComp->GetMount();
    if (mount) {
        return mount->GetController();
    }
    return nullptr;
}
