#include "SheepsAndWolvesGameMode.h"
#include "SheepsAndWolvesPlayerController.h"
#include "SheepsAndWolvesCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/DifficultyManager.h"
#include "Components/ScoreManager.h"
#include "Components/BoidsManager.h"

ASheepsAndWolvesGameMode::ASheepsAndWolvesGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	DifficultyManager = CreateDefaultSubobject< UDifficultyManager >( "DifficultyManager" );
	ScoreManager = CreateDefaultSubobject< UScoreManager >( "ScoreManager" );
	BoidsManager = CreateDefaultSubobject< UBoidsManager >( "BoidsManager" );
}