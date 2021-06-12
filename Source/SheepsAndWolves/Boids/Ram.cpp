#include "Ram.h"
#include "../Components/Abilities/RammingAbility.h"
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

ARam::ARam()
    : Super()
{
    RammingCheck = CreateDefaultSubobject< USphereComponent >( TEXT("Ramming Check") );
    RammingCheck->SetupAttachment( RootComponent );
    RammingCheck->SetCollisionEnabled( ECollisionEnabled::NoCollision );

    RammingAbility = CreateDefaultSubobject< URammingAbility >( TEXT( "Ramming Ability" ) );
    RammingAbility->RammingSphere = RammingCheck;
}

void ARam::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    ACharacter* player = UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 );
    if( player )
    {
        float playerDistance = FVector::Distance( player->GetActorLocation(), GetActorLocation() );
        if( playerDistance < RammingAbility->RammingDistance / 2.f )
        {
            RammingAbility->UseAbility();
        }
    }
}

void ARam::ApplyStun( float stunQuality )
{
    RammingAbility->CancelAbility();
    Super::ApplyStun( stunQuality );
}

bool ARam::CanBeHarmed() const
{
    return Super::CanBeHarmed() && !RammingAbility->bStartedRamming;
}

void ARam::OnDifficultyChanged( const FDifficultyWrapper& newDifficulty )
{
    Super::OnDifficultyChanged( newDifficulty );

    RammingAbility->RammingDistanceScale = newDifficulty.RammingDistanceScale;
    RammingAbility->RammingChargeUpTimeScale = newDifficulty.RammingChargeUpTimeScale;
}