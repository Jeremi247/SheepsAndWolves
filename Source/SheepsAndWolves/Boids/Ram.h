// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"
#include "Ram.generated.h"

class URammingAbility;
class USphereComponent;

UCLASS()
class SHEEPSANDWOLVES_API ARam : public ABoid
{
	GENERATED_BODY()

public:
	ARam();

	// AActor interface
	virtual void Tick( float DeltaTime ) override;
	//

	virtual void ApplyStun( float stunQuality ) override;
	virtual bool CanBeHarmed() const override;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Ramming Ability" )
		URammingAbility* RammingAbility;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Ramming Check" )
		USphereComponent* RammingCheck;

protected:
	virtual void OnDifficultyChanged( const FDifficultyWrapper& newDifficulty ) override;
};
