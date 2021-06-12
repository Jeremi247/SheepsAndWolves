#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DifficultyTableDataAsset.generated.h"

class ABoid;

USTRUCT()
struct FBoidCounter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere )
		TSubclassOf< ABoid > BoidClass;

	UPROPERTY( EditAnywhere )
		int32 SpawnLimit;
};

USTRUCT()
struct FDifficultyWrapper
{
	GENERATED_USTRUCT_BODY()

	/** Lower difficulty level to previous one if points per minute fall bellow this bar */
	UPROPERTY( EditAnywhere, Category = "General" )
		float MinPointsPerMinute;

	/** Rise difficulty level to this one if points per minute rise above this bar */
	UPROPERTY( EditAnywhere, Category = "General" )
		float ReqPointsPerMinute;

	/** Base value of boid reaction to stuns, depends on stun quality response curve */
	UPROPERTY( EditAnywhere, Category = "General" )
		float BoidStunTime;

	/** Lower range means boids sticking closer together and easier separation from neighbours */
	UPROPERTY( EditAnywhere, Category = "General" )
		float BoidNeighbourDetectionRangeScale;

	/** Ramming charge up time scale for rams */
	UPROPERTY( EditAnywhere, Category = "Rams" )
		float RammingChargeUpTimeScale;

	/** Ramming distance scale for rams */
	UPROPERTY( EditAnywhere, Category = "Rams" )
		float RammingDistanceScale;

	/** Boids to spawn on this difficulty level */
	UPROPERTY( EditAnywhere, Category = "General" )
		TArray< FBoidCounter > BoidLimits;
};

UCLASS()
class SHEEPSANDWOLVES_API UDifficultyTableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere )
		TArray< FDifficultyWrapper > Difficulties;
};
