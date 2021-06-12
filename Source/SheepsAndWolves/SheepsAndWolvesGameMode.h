// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SheepsAndWolvesGameMode.generated.h"

class UDifficultyManager;
class UScoreManager;
class UBoidsManager;

UCLASS(minimalapi)
class ASheepsAndWolvesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASheepsAndWolvesGameMode();

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Difficulty Manager" )
	UDifficultyManager* DifficultyManager;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Score Manager" )
	UScoreManager* ScoreManager;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, DisplayName = "Boids Manager" )
	UBoidsManager* BoidsManager;
};



