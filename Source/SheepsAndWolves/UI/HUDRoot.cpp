
#include "HUDRoot.h"
#include "CooldownDisplay.h"
#include "Components/VerticalBox.h"


UCooldownDisplay* UHUDRoot::AddCooldownUI()
{
    UCooldownDisplay* spawnedWidget = CreateWidget< UCooldownDisplay >( this, CooldownDisplayClass );
    
    if( spawnedWidget )
    {
        CooldownsContainer->AddChild( spawnedWidget );
        spawnedWidget->SetPadding( FMargin( 0, CooldownsSpacing ) );
    }

    return spawnedWidget;
}