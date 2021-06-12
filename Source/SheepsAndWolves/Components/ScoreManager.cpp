#include "ScoreManager.h"
#include "DifficultyManager.h"
#include "../SheepsAndWolvesGameMode.h"

UScoreManager::UScoreManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 1.f;
}

void UScoreManager::BeginPlay()
{
	Super::BeginPlay();

	ASheepsAndWolvesGameMode* gameMode = Cast< ASheepsAndWolvesGameMode >( GetWorld()->GetAuthGameMode() );
	DifficultyManager = gameMode->DifficultyManager;
}

void UScoreManager::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	ValidateScorePerMinute();
	ensureAlways( DifficultyManager.IsValid() );
	DifficultyManager->VerifyDifficultyLevel( PointsPerMinute );
}

void UScoreManager::AddScore( int score )
{
	float scoreTime = GetWorld()->GetTimeSeconds();

	ScoreTimeStamps.Emplace( scoreTime, score );
	GlobalScore += score;
}

void UScoreManager::ValidateScorePerMinute()
{
	PointsPerMinute = 0.f;

	float currentTime = GetWorld()->GetTimeSeconds();
	while( ScoreTimeStamps.Num() > 0 && ScoreTimeStamps[0].first < currentTime - 60.f )
	{
		ScoreTimeStamps.RemoveAt( 0 );
	}

	for( const std::pair< float, float >& pair : ScoreTimeStamps )
	{
		PointsPerMinute += pair.second;
	}

	if( PointsPerMinute > HighScore )
	{
		HighScore = PointsPerMinute;
	}

	GEngine->AddOnScreenDebugMessage( -1, 1.f, FColor::Red, TEXT( "Points Per Minute:" + FString::SanitizeFloat( PointsPerMinute ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 1.f, FColor::Red, TEXT( "Highest Points Per Minute:" + FString::SanitizeFloat( HighScore ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 1.f, FColor::Red, TEXT( "Highscore:" + FString::SanitizeFloat( GlobalScore ) ) );
}
