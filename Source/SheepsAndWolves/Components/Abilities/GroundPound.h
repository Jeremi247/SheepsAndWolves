// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerAbility.h"
#include "GroundPound.generated.h"

class ACharacter;
class USphereComponent;

UCLASS()
class SHEEPSANDWOLVES_API UGroundPound : public UBasePlayerAbility
{
	GENERATED_BODY()

public:
	UGroundPound();

	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Pound Speed" )
		float PoundSpeed;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Pound Radius" )
		float PoundRadius;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Pound Force" )
		float PoundForce;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Pound Offset" )
		FVector PoundOffset;
	UPROPERTY( EditAnywhere, Category = "Ability", DisplayName = "Affected Objects" )
		TArray<TEnumAsByte<EObjectTypeQuery>> AffectedObjects;

protected:
	virtual void InitializeAbility() override;
	virtual void ProcessAbility( float deltaTime ) override;
	virtual void CleanupAfterAbility() override;
	virtual bool CanBeUsed();
};
