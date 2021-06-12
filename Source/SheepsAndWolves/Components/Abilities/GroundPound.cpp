#include "GroundPound.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../Boids/Boid.h"
#include "../../SheepsAndWolvesCharacter.h"

UGroundPound::UGroundPound()
    : Super()
    , PoundSpeed( 200.f )
    , PoundRadius( 500.f )
    , PoundForce( 7000000.f )
    , PoundOffset( 0.f, 0.f, -250.f )
{

}

void UGroundPound::InitializeAbility()
{
    if( TargetPlayerCharacter.IsValid() )
    {
        if( TargetPlayerMovementComponent.IsValid() )
        {
            TargetPlayerMovementComponent->SetMovementMode( EMovementMode::MOVE_None );
        }

        return;
    }

    CleanupAfterAbility();
}

void UGroundPound::ProcessAbility( float deltaTime )
{
    if( !TargetPlayerCharacter.IsValid() )
    {
        CleanupAfterAbility();
        return;
    }

    FVector actorLocation = TargetPlayerCharacter->GetActorLocation();
    FVector targetPos = actorLocation + FVector::DownVector * PoundSpeed;
    FVector traceOffset = FVector( 0, 0, TargetPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() );

    TargetPlayerCharacter->SetActorLocation( targetPos, true, nullptr, ETeleportType::None );

    FHitResult traceResult;
    GetWorld()->LineTraceSingleByObjectType( traceResult, actorLocation - traceOffset, targetPos - traceOffset, FCollisionObjectQueryParams( ECC_TO_BITFIELD( ECC_WorldStatic ) ) );

#ifdef UE_BUILD_DEBUG
    if( bEnableDebugDraw )
    {
        DrawDebugLine( GetWorld(), actorLocation - traceOffset, targetPos - traceOffset, FColor::Green );
    }
#endif

    if( traceResult.IsValidBlockingHit() )
    {
        TArray<AActor*> actorsToIgnore;
        actorsToIgnore.Push( TargetPlayerCharacter.Get() );

        TArray<AActor*> overlappingActors;
        UKismetSystemLibrary::SphereOverlapActors( this, traceResult.Location + PoundOffset, PoundRadius, AffectedObjects, nullptr, actorsToIgnore, overlappingActors );

        FVector poundSourceLocation = traceResult.Location + PoundOffset;

#ifdef UE_BUILD_DEBUG
        if( bEnableDebugDraw )
        {
            DrawDebugSphere( GetWorld(), poundSourceLocation, PoundRadius, 20, FColor::Blue, true );
        }
#endif

        for( AActor* actor : overlappingActors )
        {
            if( ABoid* boid = Cast<ABoid>( actor ) )
            {
                float distanceFromCenter = (poundSourceLocation - boid->GetActorLocation()).Size();
                float stunQuality = 1 - ( distanceFromCenter / PoundRadius );

                boid->ApplyStun( stunQuality );
                boid->GetMovementComponent()->AddRadialForce( poundSourceLocation, PoundRadius, PoundForce, ERadialImpulseFalloff::RIF_Constant );
            }
        }

        CleanupAfterAbility();
    }
}

void UGroundPound::CleanupAfterAbility()
{
    if( TargetPlayerMovementComponent.IsValid() )
    {
        TargetPlayerMovementComponent->SetMovementMode( EMovementMode::MOVE_Walking );
    }

    Super::CleanupAfterAbility();
}

bool UGroundPound::CanBeUsed()
{
    if( TargetPlayerMovementComponent.IsValid() )
    {
        return !TargetPlayerMovementComponent->IsMovingOnGround() && Super::CanBeUsed();
    }

    return false;
}
