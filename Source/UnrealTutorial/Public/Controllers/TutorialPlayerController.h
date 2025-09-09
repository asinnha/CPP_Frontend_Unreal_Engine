// Sinha Productions

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TutorialPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTUTORIAL_API ATutorialPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	//Begin APlayerController INterface
	virtual void OnPossess(APawn* aPawn) override;

};
