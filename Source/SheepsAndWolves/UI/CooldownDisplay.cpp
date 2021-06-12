
#include "CooldownDisplay.h"

void UCooldownDisplay::SetCooldownPercent( float cooldown )
{
    if( ForegroundWidget )
    {
        float clampedCooldown = FMath::Clamp( cooldown, 0.f, 1.f );
        ForegroundWidget->SetRenderScale( FVector2D( 1.f - clampedCooldown, 1.f ) );
    }
}