// Sinha Productions

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UnrealTutorial/Public/Controllers/TutorialPlayerController.h"
#include "Widget_ActivatableBase.generated.h"


/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UNREALTUTORIAL_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintPure)
	ATutorialPlayerController* GetOwningFrontendPlayerController();

private:
	TWeakObjectPtr<ATutorialPlayerController> CachedOwningFrontendPC;

};
