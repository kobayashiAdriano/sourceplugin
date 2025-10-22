// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Interfaces/ACFEntityInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Game/ACFDamageType.h"
#include "Interfaces/ACFInteractableInterface.h"
#include "ACFWheeledVehiclePawn.generated.h"

/**
 * Represents a wheeled vehicle pawn that integrates with ACF combat and interaction systems.
 */
UCLASS()
class VEHICLESYSTEM_API AACFWheeledVehiclePawn : public AWheeledVehiclePawn, 
                    public IGenericTeamAgentInterface, public IACFEntityInterface,
                    public IACFInteractableInterface
{
    GENERATED_BODY()

public:
    /**
     * Default constructor.
     */
    AACFWheeledVehiclePawn();

    /**
     * Retrieves the combat team of the entity.
     * @return The combat team of the vehicle.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    ETeam GetEntityCombatTeam() const;
    virtual ETeam GetEntityCombatTeam_Implementation() const override { return CombatTeam; }

    /**
     * Checks if the vehicle entity is alive.
     * @return True if the vehicle is still operational, false otherwise.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool IsEntityAlive() const;
    virtual bool IsEntityAlive_Implementation() const override { return !bIsDead; }

    /**
     * Retrieves the extent radius of the entity.
     * @return The radius of the vehicle.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    float GetEntityExtentRadius() const;
    virtual float GetEntityExtentRadius_Implementation() const;

    /**
     * Assigns a combat team to the entity.
     * @param inCombatTeam The combat team to assign.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void AssignTeamToEntity(ETeam inCombatTeam);
    virtual void AssignTeamToEntity_Implementation(ETeam inCombatTeam) override;

    /**
     * Checks if the vehicle can be interacted with by a specific pawn.
     * @param Pawn The pawn attempting to interact.
     * @return True if the vehicle can be interacted with, false otherwise.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool CanBeInteracted(class APawn* Pawn);
    virtual bool CanBeInteracted_Implementation(class APawn* Pawn) override;

    /**
     * Retrieves the name of the interactable object.
     * @return The name of the vehicle.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    FText GetInteractableName();
    virtual FText GetInteractableName_Implementation() override;

    /**
     * Retrieves the last recorded damage event on this vehicle.
     * @return The last damage event details.
     */
    UFUNCTION(BlueprintPure, Category = ACF)
    FACFDamageEvent GetLastDamageInfo() const;

    /**
     * Event triggered when the team of the vehicle changes.
     */
    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnTeamChanged OnTeamChanged;

    /**
     * Retrieves the statistics component of the vehicle.
     */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UARSStatisticsComponent* GetStatisticsComponent() const { return StatisticsComp; }

    /**
     * Retrieves the damage handler component of the vehicle.
     */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFDamageHandlerComponent* GetDamageHandlerComponent() const { return DamageHandlerComp; }

    /**
     * Retrieves the mountable component of the vehicle.
     */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFMountableComponent* GetMountComponent() const { return MountComponent; }

    /**
     * Retrieves the dismount point of the vehicle.
     */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFMountPointComponent* GetDismountPoint() const { return DismountPoint; }

    /**
     * Called when the vehicle is destroyed.
     */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnVehicleDestroyed();
    virtual void OnVehicleDestroyed_Implementation();

    /**
     * Sets the generic team ID for the vehicle.
     */
    virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

    /**
     * Retrieves the generic team ID of the vehicle.
     */
    virtual FGenericTeamId GetGenericTeamId() const override;

protected:
    /**
     * Called when the game starts or when spawned.
     */
    virtual void BeginPlay() override;

    /**
     * Handles the vehicle taking damage.
     */
    virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /**
     * The combat team of the vehicle.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UARSStatisticsComponent> StatisticsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFEffectsManagerComponent> EffetsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFDamageHandlerComponent> DamageHandlerComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFMountableComponent> MountComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFMountPointComponent> DismountPoint;

    /**
     * The name of the vehicle.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FName VehicleName = "SampleCar";

private:
    /**
     * Whether the vehicle is destroyed.
     */
    UPROPERTY(Replicated)
    bool bIsDead = false;

    UFUNCTION()
    void HandleDeath();
};
