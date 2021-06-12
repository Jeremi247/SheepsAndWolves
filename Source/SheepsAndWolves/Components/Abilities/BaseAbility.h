// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbility.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHEEPSANDWOLVES_API UBaseAbility : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBaseAbility();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "CooldownTime" )
		float CooldownTime;

	UPROPERTY( EditAnywhere, Category = "DEBUG", DisplayName = "Enable Debug Draw" )
		bool bEnableDebugDraw;

	UFUNCTION()
		void UseAbility();

	virtual void CancelAbility();

protected:
	virtual void InitializeAbility() PURE_VIRTUAL( UBaseAbility::InitializeAbility(), );
	virtual void ProcessAbility( float deltaTime ) PURE_VIRTUAL( UBaseAbility::ProcessAbility(), );
	virtual void CleanupAfterAbility();
	virtual bool CanBeUsed();

	bool bAbilityInProgress;
	float CurrentCooldownTime;
};
