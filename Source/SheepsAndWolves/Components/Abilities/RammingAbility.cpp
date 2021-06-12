#include "RammingAbility.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "../../SheepsAndWolvesCharacter.h"
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

void URammingAbility::InitializeAbility()
{
    RammingCharacter = Cast< ACharacter >( GetOwner() );
    TargetPlayer = Cast< ASheepsAndWolvesCharacter >( UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ) );

    if( RammingCharacter.IsValid() && TargetPlayer.IsValid() )
    {
        UCharacterMovementComponent* movementComponent = RammingCharacter->GetCharacterMovement();
        if( movementComponent )
        {
            movementComponent->SetMovementMode( EMovementMode::MOVE_None );
        }

        CurrentDuration = 0.f;
        CurrentChargeUpTime = ChargeUpTime * RammingChargeUpTimeScale;
        bStartedRamming = false;

        return;
    }

    CleanupAfterAbility();
}

void URammingAbility::ProcessAbility( float deltaTime )
{
    if( TargetPlayer.IsValid() && RammingSphere.IsValid() && RammingCharacter.IsValid() )
    {
        if( CurrentChargeUpTime > 0.f )
        {
            CurrentChargeUpTime -= deltaTime;
            FVector direction = TargetPlayer->GetActorLocation() - RammingCharacter->GetActorLocation();
            direction.Z = 0.f;

            RammingCharacter->SetActorRotation( direction.Rotation() );
            RammingSphere->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
        }
        else
        {
            if( !bStartedRamming )
            {
                bStartedRamming = true;

                StartingPosition = RammingCharacter->GetActorLocation();
                FVector direction = RammingCharacter->GetActorForwardVector();
                EndPosition = StartingPosition + direction * RammingDistance * RammingDistanceScale;
            }

            CurrentDuration += deltaTime;

            float progress = CurrentDuration / RammingDuration;
            float clampedProgress = FMath::Clamp( progress, 0.f, 1.f );

            FVector currentTarget = FMath::Lerp( StartingPosition, EndPosition, clampedProgress );
            RammingCharacter->SetActorLocation( currentTarget, true, nullptr, ETeleportType::None );

            if( RammingSphere->IsOverlappingActor( TargetPlayer.Get() ) )
            {
                FVector forceSource = RammingCharacter->GetActorLocation() + ForceSourceOffset;
                float playerDistance = (TargetPlayer->GetActorLocation() - forceSource).Size();
                TargetPlayer->GetCharacterMovement()->AddRadialForce( forceSource, playerDistance, RammingForce, ERadialImpulseFalloff::RIF_Constant );
                TargetPlayer->ApplyStun( 1.f );
            }

            if( progress >= 1.f )
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

void URammingAbility::CleanupAfterAbility()
{
    if( RammingCharacter.IsValid() )
    {
        UCharacterMovementComponent* movementComponent = RammingCharacter->GetCharacterMovement();
        if( movementComponent )
        {
            movementComponent->SetMovementMode( EMovementMode::MOVE_Walking );
        }
    }

    if( RammingSphere.IsValid() )
    {
        RammingSphere->SetCollisionEnabled( ECollisionEnabled::NoCollision );
    }

    bStartedRamming = false;

    Super::CleanupAfterAbility();
}