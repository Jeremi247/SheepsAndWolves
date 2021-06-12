#include "BoidsManager.h"
#include "../Boids/Boid.h"
#include "../SheepsAndWolvesCharacter.h"
#include "../SheepsAndWolvesGameMode.h"
#include "DrawDebugHelpers.h"

UBoidsManager::UBoidsManager()
    : BoidSpawnHeight( 50.f )
    , RaycastSearchLength( 100.f )
{
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}

void UBoidsManager::SetBoidsSoftLimit( const TArray< FBoidCounter >& newLimits )
{
    //boids are refilled in tick
    BoidLimits = newLimits;
}

void UBoidsManager::BeginPlay()
{
    Super::BeginPlay();

    Player = Cast< ASheepsAndWolvesCharacter >( GetWorld()->GetFirstPlayerController()->GetPawn() );

    ASheepsAndWolvesGameMode* gameMode = Cast< ASheepsAndWolvesGameMode >( GetWorld()->GetAuthGameMode() );
    TWeakObjectPtr< UDifficultyManager > diffManager = gameMode->DifficultyManager;
    diffManager->OnDifficultyChanged.AddDynamic( this, &UBoidsManager::OnDifficultyChanged );
    OnDifficultyChanged( diffManager->GetCurrentDifficulty() );
}

void UBoidsManager::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    RefillBoids();
}

bool UBoidsManager::FindSafeSpawnPosition( FVector& outPosition, FRotator& outRotation )
{
    FVector playerPosition( 0, 0, 200 ); //200 for simulation purposes when player doesn't exist to be above ground
    float minimumDistance = 0.f;

    if( Player.IsValid() )
    {
        playerPosition = Player->GetActorLocation();
        minimumDistance = Player->GetVisibilityRadius();
    }

    FVector spawnDirection;

    spawnDirection.X = FMath::FRand() * 2 - 1; //random in range -1,1
    spawnDirection.Y = FMath::FRand() * 2 - 1; //random in range -1,1
    spawnDirection.Z = 0;

    spawnDirection.Normalize();

    outRotation = spawnDirection.Rotation();

    FVector spawnPosition;
    spawnPosition = spawnDirection;
    spawnPosition *= minimumDistance;

    spawnPosition += playerPosition;
    spawnPosition.Z = playerPosition.Z;

    FHitResult traceResult;

    FVector traceStart = spawnPosition;
    FVector traceEnd = spawnPosition;
    traceEnd.Z -= RaycastSearchLength;

    GetWorld()->LineTraceSingleByObjectType( traceResult, traceStart, traceEnd, FCollisionObjectQueryParams( ECC_TO_BITFIELD( ECC_WorldStatic ) ) );

    if( traceResult.IsValidBlockingHit() )
    {
        outPosition = traceResult.Location;
        outPosition.Z += BoidSpawnHeight;

        return true;
    }

    return false;
}

void UBoidsManager::RefillBoids()
{
    CleanInvalidBoidTypes();

    for( int32 i = 0; i < BoidLimits.Num(); ++i )
    {
        BoidWeakArray& foundBoids = TrackedBoids.FindOrAdd( BoidLimits[i].BoidClass );

        int32 missingBoids = BoidLimits[i].SpawnLimit - foundBoids.Num();

        TrySpawnBoids( foundBoids, missingBoids, BoidLimits[i].BoidClass );
        TryDespawnBoids( foundBoids, missingBoids * -1 );
    }
}

void UBoidsManager::NotifyAboutDeadBoid( ABoid* boid )
{
    TrackedBoids[boid->GetClass()].Remove( boid );
}

void UBoidsManager::TrySpawnBoids( BoidWeakArray& boidsArray, int32 count, TSubclassOf< ABoid > boidClass )
{
    if( count > 0 )
    {
        FVector foundPosition;
        FRotator foundRotation;
        boidsArray.Reserve( boidsArray.Num() + count );

        int32 maxFailedAttempts = 5;
        while( count > 0 && maxFailedAttempts > 0 )
        {
            if( FindSafeSpawnPosition( foundPosition, foundRotation ) )
            {
                ABoid* newBoid = GetWorld()->SpawnActor<ABoid>( boidClass, foundPosition, foundRotation );
                if( newBoid )
                {
                    maxFailedAttempts = 5;
                    newBoid->SpawnDefaultController();
                    newBoid->SetBoidsManager( this );

                    boidsArray.Push( newBoid );
                    --count;
                }
            }
            else
            {
                --maxFailedAttempts;
            }
        }
    }
}

void UBoidsManager::TryDespawnBoids( BoidWeakArray& boidsArray, int32 count )
{
    if( count > 0 )
    {
        for( int32 j = 0; j < boidsArray.Num(); ++j )
        {
            if( boidsArray[j].IsValid() )
            {
                if( boidsArray[j]->CanDespawnSafely() )
                {
                    boidsArray[j]->Destroy();
                    boidsArray.RemoveAtSwap( j );
                    --j;
                    --count;

                    if( count == 0 )
                    {
                        break;
                    }
                }
            }
            else
            {
                boidsArray.RemoveAtSwap( j );
                --j;
            }
        }
    }
}

void UBoidsManager::CleanInvalidBoidTypes()
{
    TArray< TSubclassOf< ABoid > > keysToRemove;
    TrackedBoids.GetKeys( keysToRemove );

    for( int32 i = 0; i < BoidLimits.Num(); ++i )
    {
        keysToRemove.RemoveSwap( BoidLimits[i].BoidClass );
    }

    for( const TSubclassOf< ABoid >& key : keysToRemove )
    {
        TryDespawnBoids( TrackedBoids[key], TrackedBoids[key].Num() );

        if( TrackedBoids[key].Num() == 0 )
        {
            TrackedBoids.Remove( key );
        }
    }
}

void UBoidsManager::OnDifficultyChanged( const FDifficultyWrapper& newDifficulty )
{
    SetBoidsSoftLimit( newDifficulty.BoidLimits );
}