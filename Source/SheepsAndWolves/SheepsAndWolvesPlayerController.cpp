#include "SheepsAndWolvesPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "SheepsAndWolvesCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/Abilities/DashAbility.h"
#include "Components/Abilities/GroundPound.h"
#include "UI/HUDRoot.h"

ASheepsAndWolvesPlayerController::ASheepsAndWolvesPlayerController()
	: AttackCooldown( 0.5f )
	, ControlledPawn( nullptr )
	, PawnMovementComponent( nullptr )
	, bMoveToMouseCursor( false )
{
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	DashAbility = CreateDefaultSubobject< UDashAbility >( TEXT( "Dash Ability" ) );
	GroundPoundAbility = CreateDefaultSubobject< UGroundPound >( TEXT( "Ground Pound Ability" ) );
}

void ASheepsAndWolvesPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledPawn = Cast<ASheepsAndWolvesCharacter>( GetPawn() );
	ensureAlways( ControlledPawn.IsValid() );

	PawnMovementComponent = Cast< UCharacterMovementComponent >( ControlledPawn->GetMovementComponent() );
	ensureAlways( PawnMovementComponent.IsValid() );

	HUDRoot = CreateWidget< UHUDRoot >( this, HUDClass );
	if( HUDRoot )
	{
		HUDRoot->AddToViewport();
	}

	DashAbility->RegisterAbility( InputComponent, this );
	GroundPoundAbility->RegisterAbility( InputComponent, this );
}

void ASheepsAndWolvesPlayerController::PlayerTick( float DeltaTime )
{
	Super::PlayerTick( DeltaTime );

	HandleMovement( DeltaTime );

	CurrentAttackCooldown -= DeltaTime;
}

void ASheepsAndWolvesPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction( "SetDestination", IE_Pressed, this, &ASheepsAndWolvesPlayerController::OnSetDestinationPressed );
	InputComponent->BindAction( "SetDestination", IE_Released, this, &ASheepsAndWolvesPlayerController::OnSetDestinationReleased );

	InputComponent->BindAction( "Attack", IE_Pressed, this, &ASheepsAndWolvesPlayerController::OnAttackPressed );
	InputComponent->BindAction( "Jump", IE_Pressed, this, &ASheepsAndWolvesPlayerController::OnJumpPressed );

	InputComponent->BindAxis( "MoveForward", this, &ASheepsAndWolvesPlayerController::OnForwardAxis );
	InputComponent->BindAxis( "MoveRight", this, &ASheepsAndWolvesPlayerController::OnRightAxis );
}

void ASheepsAndWolvesPlayerController::HandleMovement( float deltaTime )
{
	if( bMoveToMouseCursor && !ControlledPawn->bStunned && !bMoveWithKeyboard )
	{
		MoveToMouseCursor();
	}

	bMoveWithKeyboard = false;
}

void ASheepsAndWolvesPlayerController::MoveToMouseCursor()
{
	if( ControlledPawn->CursorToWorld )
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation( this, ControlledPawn->CursorToWorld->GetComponentLocation() );
	}
}

void ASheepsAndWolvesPlayerController::OnSetDestinationPressed()
{
	bMoveToMouseCursor = true;
}

void ASheepsAndWolvesPlayerController::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
}

void ASheepsAndWolvesPlayerController::OnAttackPressed()
{
	if( CurrentAttackCooldown < 0.f )
	{
		if( ControlledPawn->TryAttack() )
		{
			CurrentAttackCooldown = AttackCooldown;
		}
	}
}

void ASheepsAndWolvesPlayerController::OnJumpPressed()
{
	if( PawnMovementComponent.IsValid() && !PawnMovementComponent->IsFlying() && !ControlledPawn->bStunned )
	{
		ControlledPawn->Jump();
	}
}

void ASheepsAndWolvesPlayerController::OnForwardAxis( float value )
{
	if( value != 0.f )
	{
		bMoveWithKeyboard = true;
		StopMovement();
	}

	if( PawnMovementComponent.IsValid() && !ControlledPawn->bStunned )
	{
		PawnMovementComponent->AddInputVector( FVector( value, 0, 0 ) );
	}
}

void ASheepsAndWolvesPlayerController::OnRightAxis( float value )
{
	if( value != 0.f )
	{
		bMoveWithKeyboard = true;
		StopMovement();
	}

	if( PawnMovementComponent.IsValid() && !ControlledPawn->bStunned )
	{
		PawnMovementComponent->AddInputVector( FVector( 0, value, 0 ) );
	}
}