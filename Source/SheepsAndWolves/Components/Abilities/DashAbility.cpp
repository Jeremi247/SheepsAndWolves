#include "DashAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../SheepsAndWolvesCharacter.h"

UDashAbility::UDashAbility()
    : Super()
    , DashDistance( 800.f )
    , DashDuration( 0.1f )
    , CoyoteTime( 0.1f )
    , bCanBeUsed( true )
{

}

void UDashAbility::InitializeAbility()
{
    if( TargetPlayerCharacter.IsValid() )
    {
        if( TargetPlayerMovementComponent.IsValid() )
        {
            TargetPlayerMovementComponent->SetMovementMode( EMovementMode::MOVE_None );
        }

        DashStartingPosition = TargetPlayerCharacter->GetActorLocation();
        FVector direction = TargetPlayerCharacter->GetActorForwardVector();
        DashEndPosition = DashStartingPosition + direction * DashDistance;

        CurrentDuration = 0.f;
        bCanBeUsed = false;
        return;
    }

    CleanupAfterAbility();
}

void UDashAbility::ProcessAbility( float deltaTime )
{
    if( TargetPlayerCharacter.IsValid() )
    {
        CurrentDuration += deltaTime;

        float progress = CurrentDuration / DashDuration;
        float clampedProgress = FMath::Clamp( progress, 0.f, 1.f );

        FVector currentTarget = FMath::Lerp( DashStartingPosition, DashEndPosition, clampedProgress );
        TargetPlayerCharacter->SetActorLocation( currentTarget, true, nullptr, ETeleportType::None );

        if( progress >= 1.f )
        {
            bCanBeUsed = true;

            if( progress >= 1.f + CoyoteTime )
            {
                CleanupAfterAbility();
            }
        }
    }
    else
    {
        CleanupAfterAbility();
    }
}

void UDashAbility::CleanupAfterAbility()
{
    if( TargetPlayerMovementComponent.IsValid() )
    {
        TargetPlayerMovementComponent->SetMovementMode( EMovementMode::MOVE_Walking );
    }

    Super::CleanupAfterAbility();
}

bool UDashAbility::CanBeUsed()
{
    if( TargetPlayerCharacter.IsValid() )
    {
        return bCanBeUsed && !TargetPlayerCharacter->bStunned;
    }

    return bCanBeUsed;
}