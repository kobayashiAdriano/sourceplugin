// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2024. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFSummonAction.generated.h"

/**
 * 
 */
UCLASS()
class SPELLACTIONS_API UACFSummonAction : public UACFBaseAction
{
	GENERATED_BODY()

protected:

	virtual bool CanExecuteAction_Implementation(class ACharacter* owner) override;

	virtual void OnNotablePointReached_Implementation() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
	TSubclassOf<class AACFCharacter> CompanionToSummonClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
	uint8 MaxCompanionNumb = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
	float SpawnRadius = 350.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
	bool bAutoKillAfterSeconds = true;

	UPROPERTY(BlueprintReadWrite, meta = (EditCondition = bAutoKillAfterSeconds), EditAnywhere, Category = ACF)
	float AutoKillTime = 30.f;

private: 

	TArray<class AACFCharacter*> Companions;

	UFUNCTION()
	void KillCompanion(class AACFCharacter* comp);

	UFUNCTION()
	void OnCompanionDeath();

	uint8 currentCompIndex = 0;
};
