// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDRoot.generated.h"

class UVerticalBox;
class UCooldownDisplay;

UCLASS()
class SHEEPSANDWOLVES_API UHUDRoot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UCooldownDisplay* AddCooldownUI();

	UPROPERTY( BlueprintReadWrite, meta = (BindWidget) )
		UVerticalBox* CooldownsContainer;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
		TSubclassOf< UCooldownDisplay > CooldownDisplayClass;

private:
	UPROPERTY( EditAnywhere )
		float CooldownsSpacing;
};
