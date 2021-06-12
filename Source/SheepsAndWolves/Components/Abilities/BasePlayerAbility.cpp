// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerAbility.h"
#include "../../SheepsAndWolvesCharacter.h"
#include "../../SheepsAndWolvesPlayerController.h"
#include "../../UI/HUDRoot.h"
#include "../../UI/CooldownDisplay.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBasePlayerAbility::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( CooldownUI.IsValid() )
	{
		CooldownUI->SetCooldownPercent( CurrentCooldownTime / CooldownTime );
	}
}

void UBasePlayerAbility::RegisterAbility( UInputComponent* inputComponent, ASheepsAndWolvesPlayerController* playerController )
{
	if( playerController && inputComponent && !ActionName.IsNone() )
	{
		TargetPlayerController = playerController;
		if( playerController )
		{
			CooldownUI = playerController->HUDRoot->AddCooldownUI();

			TargetPlayerCharacter = Cast< ASheepsAndWolvesCharacter >( TargetPlayerController->GetPawn() );
			if( TargetPlayerCharacter.IsValid() )
			{
				TargetPlayerMovementComponent = TargetPlayerCharacter->GetCharacterMovement();
			}
		}

		inputComponent->BindAction( ActionName, IE_Pressed, this, &UBaseAbility::UseAbility );
	}
}

bool UBasePlayerAbility::CanBeUsed()
{
	if( TargetPlayerCharacter.IsValid() )
	{
		return Super::CanBeUsed() && !TargetPlayerCharacter->bStunned;
	}

	return Super::CanBeUsed();
}