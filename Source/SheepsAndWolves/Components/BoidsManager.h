// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../DataAssets/DifficultyTableDataAsset.h"
#include "BoidsManager.generated.h"

class ABoid;
class BoidGroup;
class ASheepsAndWolvesCharacter;

using BoidWeakArray = TArray< TWeakObjectPtr< ABoid > >;
using BoidTracker = TPair< TSubclassOf< ABoid >, BoidWeakArray >;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHEEPSANDWOLVES_API UBoidsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoidsManager();

	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void NotifyAboutDeadBoid( ABoid* boid );

private:
	UFUNCTION()
		void OnDifficultyChanged( const FDifficultyWrapper& newDifficulty );

	void RefillBoids();
	void CleanInvalidBoidTypes();
	void TrySpawnBoids( BoidWeakArray& boidsArray, int32 count, TSubclassOf< ABoid > boidClass );
	void TryDespawnBoids( BoidWeakArray& boidsArray, int32 count );
	void SetBoidsSoftLimit( const TArray< FBoidCounter >& newLimits );
	bool FindSafeSpawnPosition( FVector& outPosition, FRotator& outRotation );

	UPROPERTY( EditAnywhere, Category = "Boid Spawn", DisplayName = "Boid Spawn Height" )
		float BoidSpawnHeight;
	UPROPERTY( EditAnywhere, Category = "Boid Spawn", DisplayName = "Raycast Search Length" )
		float RaycastSearchLength;

	TArray< FBoidCounter > BoidLimits;
	TMap< TSubclassOf< ABoid >, BoidWeakArray > TrackedBoids;

	TWeakObjectPtr< ASheepsAndWolvesCharacter > Player;
};
