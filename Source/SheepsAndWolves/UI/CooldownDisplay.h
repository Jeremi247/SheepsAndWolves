// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h" 	
#include "Components/Image.h"
#include "CooldownDisplay.generated.h"

/**
 * 
 */
UCLASS()
class SHEEPSANDWOLVES_API UCooldownDisplay : public UUserWidget
{
	GENERATED_BODY()
		
public:
	void SetCooldownPercent( float cooldown );

	UPROPERTY( BlueprintReadWrite, meta = (BindWidget) )
		UImage* BackgroundWidget; 
	UPROPERTY( BlueprintReadWrite, meta = (BindWidget) )
		UImage* ForegroundWidget;
};
