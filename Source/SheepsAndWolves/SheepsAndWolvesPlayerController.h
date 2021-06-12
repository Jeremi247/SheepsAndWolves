// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SheepsAndWolvesPlayerController.generated.h"

class ASheepsAndWolvesCharacter;
class UCharacterMovementComponent;
class UDashAbility;
class UGroundPound;
class UHUDRoot;

UCLASS()
class ASheepsAndWolvesPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASheepsAndWolvesPlayerController();

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Dash Ability" )
		UDashAbility* DashAbility;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Ground Pound Ability" )
		UGroundPound* GroundPoundAbility;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "HUD" )
		TSubclassOf< UHUDRoot > HUDClass;
	UHUDRoot* HUDRoot;

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick( float DeltaTime ) override;
	virtual void SetupInputComponent() override;

private:
	void HandleMovement( float deltaTime );
	void MoveToMouseCursor();

	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnAttackPressed();
	void OnJumpPressed();
	void OnForwardAxis( float value );
	void OnRightAxis( float value );

	UPROPERTY( EditAnywhere, Category = "Attack", DisplayName = "Attack Cooldown" )
		float AttackCooldown;

	float CurrentAttackCooldown;

	TWeakObjectPtr< ASheepsAndWolvesCharacter > ControlledPawn;
	TWeakObjectPtr< UCharacterMovementComponent > PawnMovementComponent;

	bool bMoveToMouseCursor;
	bool bMoveWithKeyboard;
};


