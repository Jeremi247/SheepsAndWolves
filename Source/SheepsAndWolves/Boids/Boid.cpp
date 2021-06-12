#include "Boid.h"
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "../SheepsAndWolvesGameMode.h"
#include "../SheepsAndWolvesCharacter.h"
#include "DrawDebugHelpers.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "../Components/BoidsManager.h"
#include "Curves/CurveFloat.h"

ABoid::ABoid()
	: Super()
	, OverlappingObstacles( 0 )
	, CurrentStunLength( 0.f )
	, MinSpeed( 300.f )
	, MaxSpeed( 700.f )
	, MinSpeedInPanicMode( 400.f )
	, MaxSpeedInPanicMode( 900.f )
	, MaxSteeringForce( 1000.f )
	, PlayerAvoidanceDistanceOffset( 1000.f )
	, PlayerAvoidanceCurvePower( 0.5f )
	, BoidForwardWeight( 1.f )
	, FlockForwardWeight( 1.f )
	, AvoidanceWeight( 2.f )
	, CollisionAvoidanceWeight( 4.f )
	, ViewAngle( 270.f )
	, AvoidanceRadiusPercent( 0.5f )
	, EnableDebugDraw( true )
	, StunRotationSpeed( 1000.f )
	, PanicModeSearchRadius( 1000.f )
	, CollisionDetecionCheckAngle( 30.f )
	, StunLength( 0.f )
{
#ifdef UE_BUILD_DEBUG
	PrimaryActorTick.bCanEverTick = true;
#endif
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GravityScale = 7.0f;

	NeighbourDetection = CreateDefaultSubobject<USphereComponent>( TEXT( "SphereComponent" ) );
	NeighbourDetection->SetSphereRadius( 300.f );
	NeighbourDetection->SetupAttachment( RootComponent );

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void ABoid::BeginPlay()
{
	CacheComponents();

	TWeakObjectPtr< ASheepsAndWolvesGameMode > gameMode = GetWorld()->GetAuthGameMode< ASheepsAndWolvesGameMode >();
	TWeakObjectPtr< UDifficultyManager > difficultyManager = gameMode->DifficultyManager;
	difficultyManager->OnDifficultyChanged.AddDynamic( this, &ABoid::OnDifficultyChanged );
	OnDifficultyChanged( difficultyManager->GetCurrentDifficulty() );

	NeighbourDetection->OnComponentBeginOverlap.AddDynamic( this, &ABoid::OnOverlap );
	NeighbourDetection->OnComponentEndOverlap.AddDynamic( this, &ABoid::OnOverlapEnd );

	// example angle 270
	// 270 / 180 = 1.5
	// 2 - 1.5 = 0.5
	// -1 + 0.5 = -0.5 <- this is value to compare with dot product of other boid position
	CutoffViewPercent = -1.f + (2.f - ViewAngle / 180.f);

	Super::BeginPlay();
}

void ABoid::CacheComponents()
{
	MovementComponent = GetCharacterMovement();
}

void ABoid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	UpdateMovement( DeltaTime );
}

void ABoid::Destroyed()
{
	Super::Destroyed();

	if( BoidsManager.IsValid() )
	{
		BoidsManager->NotifyAboutDeadBoid( this );
	}
}

void ABoid::UpdateMovement( float deltaTime )
{
	if( CurrentStunLength > 0.f )
	{
		HandleStun( deltaTime );
		return;
	}

	HandlePanicMode();
	HandleMovement();
}

void ABoid::UpdateSteeringVectors()
{
	UpdatePlayerAvoidanceSteeringVector();
	UpdateBaseDirectionSteeringVector();
	UpdateCollisionSteeringVector();
	UpdateFlockSteeringVectors();
}

void ABoid::UpdatePlayerAvoidanceSteeringVector()
{
	PlayerAvoidanceDirection = FVector::ZeroVector;

	ACharacter* player = UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 );
	if( player )
	{
		PlayerAvoidanceDirection = GetActorLocation() - player->GetActorLocation();
	}
}

void ABoid::UpdateBaseDirectionSteeringVector()
{
	BaseDirectionVector = GetActorForwardVector();

	if( bInPanicMode && PanicTarget.IsValid() )
	{
		BaseDirectionVector = PanicTarget->GetActorLocation() - GetActorLocation();

#ifdef UE_BUILD_DEBUG
		if( EnableDebugDraw )
		{
			DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + BaseDirectionVector, FColor::Blue, false, -1.f, 0, 5.f );
		}
#endif
	}
}

void ABoid::UpdateCollisionSteeringVector()
{
	CollisionAvoidanceDirection = FVector::ZeroVector;

	if( OverlappingObstacles > 0 )
	{
		FVector baseRaycastDirection = GetActorForwardVector();
		FHitResult traceResult;

		TraceSingleCollision( 0.f, traceResult, baseRaycastDirection );
		TraceCollisionDetection( 1.f );
		TraceCollisionDetection( -1.f );
	}
}

void ABoid::UpdateFlockSteeringVectors()
{
	LocalFlockForward = FVector::ZeroVector;
	NeighboursAvoidanceDirection = FVector::ZeroVector;

	for( int32 i = 0; i < Neighbours.Num(); ++i )
	{
		const TWeakObjectPtr< ABoid >& neighbour = Neighbours[i];

		if( !neighbour.IsValid() )
		{
			Neighbours.RemoveAtSwap( i );
			--i;
			continue;
		}

		FVector direction = neighbour->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		float dot = FVector::DotProduct( GetActorForwardVector(), direction );

		if( dot > CutoffViewPercent )
		{
			LocalFlockForward += neighbour->GetActorForwardVector();
		}

#ifdef UE_BUILD_DEBUG
		if( EnableDebugDraw )
		{
			FColor drawColor = FColor::Red;
			if( dot > CutoffViewPercent )
			{
				drawColor = FColor::Green;
			}

			DrawDebugLine( GetWorld(), GetActorLocation(), neighbour->GetActorLocation(), drawColor );
		}
#endif

		FVector offset = neighbour->GetActorLocation() - GetActorLocation();

		if( offset.Size() < NeighbourDetection->GetScaledSphereRadius() * AvoidanceRadiusPercent )
		{
			if( offset.Size() == 0 )
			{
				NeighboursAvoidanceDirection.X = FMath::FRand();
				NeighboursAvoidanceDirection.Y = FMath::FRand();
			}
			else
			{
				NeighboursAvoidanceDirection -= offset / offset.SizeSquared();
			}
		}
	}
}

void ABoid::TraceSingleCollision( float direction, FHitResult& outTraceResult, FVector& outRaycastDirection )
{
	outRaycastDirection = outRaycastDirection.RotateAngleAxis( CollisionDetecionCheckAngle * direction, FVector::UpVector );
	FVector searchTarget = GetActorLocation() + outRaycastDirection * NeighbourDetection->GetScaledSphereRadius();

	GetWorld()->LineTraceSingleByObjectType(
		outTraceResult,
		GetActorLocation(),
		searchTarget,
		FCollisionObjectQueryParams( ECC_TO_BITFIELD( ECC_WorldStatic ) ) );

	if( outTraceResult.bBlockingHit )
	{
		FVector targetDirection = GetActorLocation() - outTraceResult.ImpactPoint;
		targetDirection.Normalize();
		CollisionAvoidanceDirection += targetDirection;
	}

#ifdef UE_BUILD_DEBUG
	if( EnableDebugDraw )
	{
		FColor color = outTraceResult.bBlockingHit ? FColor::Yellow : FColor::Black;
		DrawDebugLine( GetWorld(), GetActorLocation(), searchTarget, color, false, -1.f, 0, 5.f );
		DrawDebugSphere( GetWorld(), outTraceResult.ImpactPoint, 5.f, 3.f, FColor::Emerald, false, -1.f, 0, 5.f );
	}
#endif
}

void ABoid::TraceCollisionDetection( float direction )
{
	FHitResult traceResult;
	FVector raycastDirection = GetActorForwardVector();

	do
	{
		TraceSingleCollision( direction, traceResult, raycastDirection );
	} while( traceResult.bBlockingHit );
}

void ABoid::HandleStun( float deltaTime )
{
	CurrentStunLength -= deltaTime;
	FVector rotation = GetActorRotation().Euler();
	rotation.Z += StunRotationSpeed * deltaTime;

	SetActorRotation( FRotator::MakeFromEuler( rotation ) );
}

void ABoid::HandlePanicMode()
{
	if( bInPanicMode && HasAnyNeighbours() )
	{
		bInPanicMode = false;
		PanicTarget = nullptr;
	}

	if( !bInPanicMode && !HasAnyNeighbours() )
	{
		bInPanicMode = true;
	}

	if( bInPanicMode && !PanicTarget.IsValid() )
	{
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Push( this );
		TArray<AActor*> overlappingActors;
		UKismetSystemLibrary::SphereOverlapActors( this, GetActorLocation(), PanicModeSearchRadius, PanicModeSearchTargets, nullptr, actorsToIgnore, overlappingActors );

#ifdef UE_BUILD_DEBUG
		if( EnableDebugDraw )
		{
			DrawDebugSphere( GetWorld(), GetActorLocation(), PanicModeSearchRadius, 20, FColor::Red, false, -1.f, 0, 5.f );
		}
#endif

		if( overlappingActors.Num() > 0 )
		{
			PanicTarget = Cast< ABoid >( overlappingActors[0] );

			if( PanicTarget.IsValid() )
			{
				float distance = (PanicTarget->GetActorLocation() - GetActorLocation()).Size();

				for( AActor* actor : overlappingActors )
				{
					float newDistance = (actor->GetActorLocation() - GetActorLocation()).Size();

					if( newDistance < distance )
					{
						distance = newDistance;
						PanicTarget = Cast< ABoid >( actor );
					}
				}
			}
		}
	}
}

void ABoid::HandleMovement()
{
	UpdateSteeringVectors();

	FVector acceleration = FVector::ZeroVector;
	float playerAvoidanceStrength = PlayerAvoidanceCurvePower / (PlayerAvoidanceDirection.Size() / PlayerAvoidanceDistanceOffset);

	acceleration += SteerTowards( BaseDirectionVector ) * BoidForwardWeight;
	acceleration += SteerTowards( LocalFlockForward ) * FlockForwardWeight;
	acceleration += SteerTowards( NeighboursAvoidanceDirection ) * AvoidanceWeight;
	if( PlayerAvoidanceDirection.Size() < PlayerAvoidanceDistanceOffset * 2 )
	{
		acceleration += SteerTowards( PlayerAvoidanceDirection ) * playerAvoidanceStrength;
	}
	if( CollisionAvoidanceDirection.Size() > 0 )
	{
		acceleration += SteerTowards( CollisionAvoidanceDirection ) * CollisionAvoidanceWeight;
	}

	FVector velocity = MovementComponent->Velocity + (acceleration * GetWorld()->GetDeltaSeconds());
	float speed = velocity.Size();
	FVector direction = velocity / speed;
	float minSpeed = bInPanicMode ? MinSpeedInPanicMode : MinSpeed;
	float maxSpeed = bInPanicMode ? MaxSpeedInPanicMode : MaxSpeed;
	speed = FMath::Clamp( speed, minSpeed, maxSpeed );
	velocity = direction * speed;

#ifdef UE_BUILD_DEBUG
	if( EnableDebugDraw )
	{
		DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + velocity, FColor::Green, false, -1.f, 0, 5.f );
		DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + acceleration, FColor::Silver, false, -1.f, 0, 5.f );
		DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + direction * 100.f, FColor::Cyan, false, -1.f, 0, 5.f );
	}
#endif

	MovementComponent->MaxWalkSpeed = speed;
	MovementComponent->RequestPathMove( direction );
}

void ABoid::ApplyStun( float stunQuality )
{
	bStunned = true;
	float stunStrength = StunQualityResponseCurve->GetFloatValue( stunQuality );
	CurrentStunLength = StunLength * stunStrength;
}

bool ABoid::CanDespawnSafely() const
{
	ASheepsAndWolvesCharacter* player = Cast< ASheepsAndWolvesCharacter >( UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ) );

	if( player )
	{
		FVector distanceFromPlayer = player->GetActorLocation() - GetActorLocation();

		if( distanceFromPlayer.Size() < player->GetVisibilityRadius() )
		{
			return false;
		}
	}

	return true;
}

bool ABoid::HasAnyNeighbours() const
{
	return Neighbours.Num() > 0;
}

bool ABoid::CanBeHarmed() const
{
	return !HasAnyNeighbours();
}

void ABoid::SetBoidsManager( UBoidsManager* boidsManager )
{
	BoidsManager = boidsManager;
}

FVector ABoid::SteerTowards( FVector vector )
{
	vector.Normalize();
	vector *= MaxSpeed;
	vector -= MovementComponent->Velocity;
	return vector.GetClampedToMaxSize( MaxSteeringForce );
}

void ABoid::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult )
{
	if( OtherActor == this )
	{
		return;
	}

	ECollisionResponse response = OtherComp->GetCollisionResponseToChannel( ECollisionChannel::ECC_WorldStatic );
	if( response == ECollisionResponse::ECR_Block )
	{
		++OverlappingObstacles;
	}

	if( ABoid* boid = Cast<ABoid>( OtherActor ) )
	{
		if( boid != this )
		{
			Neighbours.AddUnique( boid );
		}
	}
}

void ABoid::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex )
{
	if( OtherActor == this )
	{
		return;
	}

	ECollisionResponse response = OtherComp->GetCollisionResponseToChannel( ECollisionChannel::ECC_WorldStatic );
	if( response == ECollisionResponse::ECR_Block )
	{
		--OverlappingObstacles;
	}

	if( ABoid* boid = Cast<ABoid>( OtherActor ) )
	{
		Neighbours.Remove( boid );
	}
}

void ABoid::OnDifficultyChanged( const FDifficultyWrapper& newDifficulty )
{
	StunLength = newDifficulty.BoidStunTime;
	NeighbourDetection->SetWorldScale3D( FVector( newDifficulty.BoidNeighbourDetectionRangeScale ) );
}