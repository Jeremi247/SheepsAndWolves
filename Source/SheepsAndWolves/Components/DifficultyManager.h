#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../DataAssets/DifficultyTableDataAsset.h"
#include "DifficultyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam( FDifficultyChangedSignature, UDifficultyManager, OnDifficultyChanged, const FDifficultyWrapper&, DifficultyWrapper );

UCLASS( ClassGroup = (Custom) )
class SHEEPSANDWOLVES_API UDifficultyManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UDifficultyManager();

	void VerifyDifficultyLevel( float pointsPerMinute );
	const FDifficultyWrapper& GetCurrentDifficulty() const;

	FDifficultyChangedSignature OnDifficultyChanged;

	UPROPERTY( EditAnywhere, Category = "Data", DisplayName = "Difficulties Data Asset" )
		UDifficultyTableDataAsset* DifficultyDataAsset;

protected:
	virtual void BeginPlay() override;

private:
	void TryLowerDifficulty( int32& newDiffIndex, float pointsPerMinute );
	void TryRiseDifficulty( int32& newDiffIndex, float pointsPerMinute );
	void SetDifficultyLevel( int32 newDifficulty );

	int32 CurrentDifficultyIndex;
};
