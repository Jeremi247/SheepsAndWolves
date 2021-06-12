// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SheepsAndWolvesCharacter.generated.h"

class UBoxComponent;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;
class UDecalComponent;
class UScoreManager;
class ABoid;

UCLASS(Blueprintable)
class ASheepsAndWolvesCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASheepsAndWolvesCharacter();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Misc, DisplayName = "Consumption Area" )
		UBoxComponent* ConsumptionArea;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Misc, DisplayName = "Visibility Area" )
		USphereComponent* VisibilityArea;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, DisplayName = "Top Down Camera Component" )
		UCameraComponent* TopDownCameraComponent;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, DisplayName = "Camera Boom" )
		USpringArmComponent* CameraBoom;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, DisplayName = "Cursors To World" )
		UDecalComponent* CursorToWorld;
	UPROPERTY( EditAnywhere, Category = Repulsion, DisplayName = "Repulsion Force" )
		float RepulsionForce;

	bool TryAttack();
	void ApplyStun( float stunQuality );

	float GetVisibilityRadius() const;

	bool bStunned;

private:
	void Consume( const TWeakObjectPtr< ABoid >& boid );

	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Stun Quality Response Curve" )
		UCurveFloat* StunQualityResponseCurve;
	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Stun Length" )
		float StunLength;
	float CurrentStunLength;

	TWeakObjectPtr< UScoreManager > ScoreManager;
	TArray< TWeakObjectPtr< ABoid > > NearbyBoids;
	bool bRepulsiveForceEnabled;
};

