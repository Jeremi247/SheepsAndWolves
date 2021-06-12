// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "BasePlayerAbility.generated.h"

class ASheepsAndWolvesPlayerController;
class ASheepsAndWolvesCharacter;
class UCharacterMovementComponent;
class UCooldownDisplay;

UCLASS()
class SHEEPSANDWOLVES_API UBasePlayerAbility : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void RegisterAbility( UInputComponent* inputComponent, ASheepsAndWolvesPlayerController* playerController );

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Action Name" )
		FName ActionName;

protected:
	virtual bool CanBeUsed() override;

	TWeakObjectPtr< ASheepsAndWolvesPlayerController > TargetPlayerController;
	TWeakObjectPtr< ASheepsAndWolvesCharacter > TargetPlayerCharacter;
	TWeakObjectPtr< UCharacterMovementComponent > TargetPlayerMovementComponent;
	TWeakObjectPtr< UCooldownDisplay > CooldownUI;
};
