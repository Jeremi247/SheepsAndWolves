#pragma once

#include "CoreMinimal.h"
#include "BasePlayerAbility.h"
#include "DashAbility.generated.h"

UCLASS()
class SHEEPSANDWOLVES_API UDashAbility : public UBasePlayerAbility
{
	GENERATED_BODY()

public:
	UDashAbility();

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Dash Distance" )
		float DashDistance;

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Dash Duration" )
		float DashDuration;

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Coyote Time" )
		float CoyoteTime;

protected:
	virtual void InitializeAbility() override;
	virtual void ProcessAbility( float deltaTime ) override;
	virtual void CleanupAfterAbility() override;
	virtual bool CanBeUsed();

private:
	FVector DashStartingPosition;
	FVector DashEndPosition;
	float CurrentDuration;
	bool bCanBeUsed;
};
