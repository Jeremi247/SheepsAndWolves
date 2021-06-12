#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Components/DifficultyManager.h"
#include "Boid.generated.h"

class USphereComponent;
class BoidGroup;
class UBoidGroupsController;
class ASheepsAndWolvesCharacter;
class UBoidsManager;
class UCurveFloat;

UCLASS()
class SHEEPSANDWOLVES_API ABoid : public ACharacter
{
	GENERATED_BODY()

public:
	ABoid();

	// AActor interface
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaTime ) override;
	virtual void Destroyed() override;
	//

	void UpdateMovement( float deltaTime );

	virtual void ApplyStun( float stunQuality );
	bool CanDespawnSafely() const;

	bool HasAnyNeighbours() const;
	virtual bool CanBeHarmed() const;

	void SetBoidsManager( UBoidsManager* boidsManager );

private:
	void CacheComponents();
	FVector SteerTowards( FVector vector );

	void UpdateSteeringVectors();
	void UpdatePlayerAvoidanceSteeringVector();
	void UpdateBaseDirectionSteeringVector();
	void UpdateCollisionSteeringVector();
	void UpdateFlockSteeringVectors();

	void TraceSingleCollision( float direction, FHitResult& outTraceResult, FVector& outRaycastDirection );
	void TraceCollisionDetection( float direction );

	void HandleStun( float deltaTime );
	void HandleMovement();
	void HandlePanicMode();

	UFUNCTION()
		void OnOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult );

	UFUNCTION()
		void OnOverlapEnd(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex );

	TWeakObjectPtr< UCharacterMovementComponent > MovementComponent;
	TWeakObjectPtr< UBoidsManager > BoidsManager;

	TArray< TWeakObjectPtr< ABoid > > Neighbours;
	TSharedPtr< BoidGroup > MyGroup;
	int32 OverlappingObstacles;

	FVector BaseDirectionVector;
	FVector LocalFlockForward;
	FVector PlayerAvoidanceDirection;
	FVector NeighboursAvoidanceDirection;
	FVector CollisionAvoidanceDirection;

	float CutoffViewPercent;

	float CurrentStunLength;
	bool bStunned;
	bool bInPanicMode;
	TWeakObjectPtr< ABoid > PanicTarget;

protected:

	UFUNCTION()
		virtual void OnDifficultyChanged( const FDifficultyWrapper& newDifficulty );

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		USphereComponent* NeighbourDetection;

	UPROPERTY( EditAnywhere, Category = "Speed controll", DisplayName = "Min Speed" )
		float MinSpeed;
	UPROPERTY( EditAnywhere, Category = "Speed controll", DisplayName = "Max Speed" )
		float MaxSpeed;
	UPROPERTY( EditAnywhere, Category = "Speed controll", DisplayName = "Min Speed In Panic Mode" )
		float MinSpeedInPanicMode;
	UPROPERTY( EditAnywhere, Category = "Speed controll", DisplayName = "Max Speed In Panic Mode" )
		float MaxSpeedInPanicMode;
	UPROPERTY( EditAnywhere, Category = "Speed controll", DisplayName = "Max Steering Force" )
		float MaxSteeringForce;

	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Player Avoidance Distance Offset" )
		float PlayerAvoidanceDistanceOffset;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Player Avoidance Curve Power" )
		float PlayerAvoidanceCurvePower;

	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Boid Forward Weight" )
		float BoidForwardWeight;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Flock Forward Weight" )
		float FlockForwardWeight;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Avoidance Weight" )
		float AvoidanceWeight;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Collision Avoidance Weight" )
		float CollisionAvoidanceWeight;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "View Angle", meta = ( ClampMin = 0.f, ClampMax = 360.f ) )
		float ViewAngle;
	UPROPERTY( EditAnywhere, Category = "Steering", DisplayName = "Avoidance Radius Percent" )
		float AvoidanceRadiusPercent;

	UPROPERTY( EditAnywhere, Category = "Debug", DisplayName = "Enable Debug Draw" )
		bool EnableDebugDraw;

	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Stun Quality Response Curve" )
		UCurveFloat* StunQualityResponseCurve;
	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Stun Rotation Speed" )
		float StunRotationSpeed;
	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Panic Mode Search Radius" )
		float PanicModeSearchRadius;
	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Panic Mode Search Targets" )
		TArray<TEnumAsByte<EObjectTypeQuery>> PanicModeSearchTargets;
	UPROPERTY( EditAnywhere, Category = "Misc", DisplayName = "Collision Detecion Check Angle" )
		float CollisionDetecionCheckAngle;
	float StunLength;
};
