#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "RammingAbility.generated.h"

class ASheepsAndWolvesCharacter;
class USphereComponent;

UCLASS()
class SHEEPSANDWOLVES_API URammingAbility : public UBaseAbility
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Charge Up Time" )
		float ChargeUpTime;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Ramming Distance" )
		float RammingDistance;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Ramming Duration" )
		float RammingDuration;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Ramming Force" )
		float RammingForce;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Ramming Force" )
		FVector ForceSourceOffset;

	TWeakObjectPtr< USphereComponent > RammingSphere;
	bool bStartedRamming;
	float RammingDistanceScale;
	float RammingChargeUpTimeScale;

protected:
	virtual void InitializeAbility() override;
	virtual void ProcessAbility( float deltaTime ) override;
	virtual void CleanupAfterAbility() override;

private:
	TWeakObjectPtr< ACharacter > RammingCharacter;
	TWeakObjectPtr< ASheepsAndWolvesCharacter > TargetPlayer;

	float CurrentChargeUpTime;
	FVector StartingPosition;
	FVector EndPosition;
	float CurrentDuration;
};
