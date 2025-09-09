// Sinha Productions


#include "Widgets/Widget_ActivatableBase.h"

ATutorialPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
    if(!CachedOwningFrontendPC.IsValid())
    {
        CachedOwningFrontendPC = GetOwningPlayer<ATutorialPlayerController>();
	}
    return CachedOwningFrontendPC.IsValid() ? CachedOwningFrontendPC.Get() : nullptr;
}
