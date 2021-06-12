#include "BaseAbility.h"

UBaseAbility::UBaseAbility()
	: CooldownTime( 0.f )
	, bAbilityInProgress( false )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBaseAbility::UseAbility()
{
	if( CanBeUsed() && CurrentCooldownTime <= 0.f )
	{
		bAbilityInProgress = true;
		CurrentCooldownTime = CooldownTime;
		InitializeAbility();
	}
}

void UBaseAbility::CancelAbility()
{
	if( bAbilityInProgress )
	{
		CleanupAfterAbility();
	}
}

void UBaseAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentCooldownTime -= DeltaTime;

	if( bAbilityInProgress )
	{
		ProcessAbility( DeltaTime );
	}
}

void UBaseAbility::CleanupAfterAbility()
{
	bAbilityInProgress = false;
}

bool UBaseAbility::CanBeUsed()
{
	return !bAbilityInProgress;
}