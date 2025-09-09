// Sinha Productions


#include "FrontendFunctionLibrary.h"
#include "UnrealTutorial/Public/DeveloperSettings/FrontendDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UFrontendFunctionLibrary::GetFrontednSoftWidgetClassByTag( UPARAM(meta = (Categories="Frontend.Widgets")) FGameplayTag InWidgetTag)
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	checkf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("Could not find the corrosponding widget under the tag %s"), *InWidgetTag.ToString());


	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}
