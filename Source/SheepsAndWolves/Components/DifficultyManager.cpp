#include "DifficultyManager.h"

UDifficultyManager::UDifficultyManager()
	: CurrentDifficultyIndex( -1 )
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDifficultyManager::BeginPlay()
{
	Super::BeginPlay();

	VerifyDifficultyLevel( 0 );
}

const FDifficultyWrapper& UDifficultyManager::GetCurrentDifficulty() const
{
	if( CurrentDifficultyIndex >= 0 )
	{
		return DifficultyDataAsset->Difficulties[CurrentDifficultyIndex];
	}
	else
	{
		return DifficultyDataAsset->Difficulties[0];
	}
}

void UDifficultyManager::VerifyDifficultyLevel( float pointsPerMinute )
{
	int32 newDifficultyIndex = CurrentDifficultyIndex;

	if( CurrentDifficultyIndex > 0 )
	{
		TryLowerDifficulty( newDifficultyIndex, pointsPerMinute );
	}

	if( newDifficultyIndex == CurrentDifficultyIndex )
	{
		TryRiseDifficulty( newDifficultyIndex, pointsPerMinute );
	}

	if( newDifficultyIndex < 0 )
	{
		newDifficultyIndex = 0;
	}

	if( newDifficultyIndex != CurrentDifficultyIndex )
	{
		SetDifficultyLevel( newDifficultyIndex );
	}
}

void UDifficultyManager::TryLowerDifficulty( int32& newDiffIndex, float pointsPerMinute )
{
	for( int32 i = CurrentDifficultyIndex; i >= 0; --i )
	{
		const FDifficultyWrapper& currentDifficulty = DifficultyDataAsset->Difficulties[i];

		newDiffIndex = i;

		if( pointsPerMinute >= currentDifficulty.MinPointsPerMinute )
		{
			return;
		}
	}
}

void UDifficultyManager::TryRiseDifficulty( int32& newDiffIndex, float pointsPerMinute )
{
	for( int32 i = CurrentDifficultyIndex + 1; i < DifficultyDataAsset->Difficulties.Num(); ++i )
	{
		const FDifficultyWrapper& currentDifficulty = DifficultyDataAsset->Difficulties[i];

		if( pointsPerMinute < currentDifficulty.ReqPointsPerMinute )
		{
			return;
		}

		newDiffIndex = i;
	}
}

void UDifficultyManager::SetDifficultyLevel( int32 newDifficulty )
{
	CurrentDifficultyIndex = newDifficulty;
	OnDifficultyChanged.Broadcast( DifficultyDataAsset->Difficulties[newDifficulty] );
}