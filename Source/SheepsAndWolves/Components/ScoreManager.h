#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <utility>
#include "ScoreManager.generated.h"

class UDifficultyManager;

UCLASS( ClassGroup = (Custom) )
class SHEEPSANDWOLVES_API UScoreManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UScoreManager();

public:
	void AddScore( int score );

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void ValidateScorePerMinute();

	TWeakObjectPtr< UDifficultyManager > DifficultyManager;

	TArray< std::pair< float, float > > ScoreTimeStamps;
	float GlobalScore;
	float HighScore;
	float PointsPerMinute;
};
