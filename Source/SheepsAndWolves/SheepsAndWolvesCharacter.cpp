#include "SheepsAndWolvesCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Boids/Boid.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include "DrawDebugHelpers.h"
#include "SheepsAndWolvesGameMode.h"
#include "Components/ScoreManager.h"

ASheepsAndWolvesCharacter::ASheepsAndWolvesCharacter()
	: RepulsionForce( 40000000.f )
	, bRepulsiveForceEnabled( false )
{
	//pre generated
	GetCapsuleComponent()->InitCapsuleSize( 42.f, 96.0f );

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator( 0.f, 640.f, 0.f );
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT( "CameraBoom" ) );
	CameraBoom->SetupAttachment( RootComponent );
	CameraBoom->SetUsingAbsoluteRotation( true );
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation( FRotator( -60.f, 0.f, 0.f ) );
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>( TEXT( "TopDownCamera" ) );
	TopDownCameraComponent->SetupAttachment( CameraBoom, USpringArmComponent::SocketName );
	TopDownCameraComponent->bUsePawnControlRotation = false;

	CursorToWorld = CreateDefaultSubobject<UDecalComponent>( "CursorToWorld" );
	CursorToWorld->SetupAttachment( RootComponent );
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset( TEXT( "Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'" ) );
	if( DecalMaterialAsset.Succeeded() )
	{
		CursorToWorld->SetDecalMaterial( DecalMaterialAsset.Object );
	}
	CursorToWorld->DecalSize = FVector( 16.0f, 32.0f, 32.0f );
	CursorToWorld->SetRelativeRotation( FRotator( 90.0f, 0.0f, 0.0f ).Quaternion() );

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	// end pre generated

	VisibilityArea = CreateDefaultSubobject< USphereComponent >( TEXT( "VisibilityArea" ) );
	VisibilityArea->SetupAttachment( RootComponent );
	VisibilityArea->SetCollisionEnabled( ECollisionEnabled::NoCollision );

	ConsumptionArea = CreateDefaultSubobject< UBoxComponent >( TEXT( "ConsumptionArea" ) );
	ConsumptionArea->SetupAttachment( RootComponent );
}

void ASheepsAndWolvesCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASheepsAndWolvesGameMode* gameMode = Cast< ASheepsAndWolvesGameMode >( GetWorld()->GetAuthGameMode() );
	ScoreManager = gameMode->ScoreManager;
}

void ASheepsAndWolvesCharacter::Tick( float DeltaSeconds )
{
	Super::Tick( DeltaSeconds );

	if( CurrentStunLength > 0.f )
	{
		CurrentStunLength -= DeltaSeconds;
	}
	else if( bStunned )
	{
		bStunned = false;
		GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_Walking );
	}

	if( CursorToWorld != nullptr )
	{
		if( APlayerController* PC = Cast<APlayerController>( GetController() ) )
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor( ECC_Visibility, true, TraceHitResult );
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation( TraceHitResult.Location );
			CursorToWorld->SetWorldRotation( CursorR );
		}
	}
}

void ASheepsAndWolvesCharacter::ApplyStun( float stunQuality )
{
	bStunned = true;
	float stunStrength = StunQualityResponseCurve->GetFloatValue( stunQuality );
	CurrentStunLength = StunLength * stunStrength;

	GetController()->StopMovement();
}

bool ASheepsAndWolvesCharacter::TryAttack()
{
	TSet< AActor* > overlappingActors;
	ConsumptionArea->GetOverlappingActors( overlappingActors );

	for( AActor* actor : overlappingActors )
	{
		if( ABoid* boid = Cast< ABoid >( actor ) )
		{
			if( boid->CanBeHarmed() )
			{
				Consume( boid );
				return true;
			}
		}
	}

	return false;
}

float ASheepsAndWolvesCharacter::GetVisibilityRadius() const
{
	return VisibilityArea->GetScaledSphereRadius();
}

void ASheepsAndWolvesCharacter::Consume( const TWeakObjectPtr< ABoid >& boid )
{
	if( boid.IsValid() )
	{
		boid->Destroy();
		ensureAlways( ScoreManager.IsValid() );
		ScoreManager->AddScore( 1.f );
	}
}