// Sinha Productions


#include "AsyncActions/AsyncAction_PushSoftWidgets.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Subsystems/FrontendUISubsystems.h"

UAsyncAction_PushSoftWidgets* UAsyncAction_PushSoftWidgets::PushSoftWidget(const UObject* WorldContextObject, APlayerController* OwningPlayerController, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget)
{

	checkf(!InSoftWidgetClass.IsNull(),TEXT("PushSoftWidgetToStack was passed a null soft widget class."))

	if(GEngine)
	{
		if(UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{

			UAsyncAction_PushSoftWidgets* Node = NewObject<UAsyncAction_PushSoftWidgets>();
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

			Node->RegisterWithGameInstance(World);

			return Node;

		}
	
	}

	return nullptr;
}

void UAsyncAction_PushSoftWidgets::Activate()
{

	UFrontendUISubsystems* FrontendUISubsystem = UFrontendUISubsystems::Get(CachedOwningWorld.Get());
	FrontendUISubsystem->PushSoftWidgetToStackAsync(
		CachedWidgetStackTag,
		CachedSoftWidgetClass,
		[this](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			switch(InPushState)
			{
				case EAsyncPushWidgetState::OnCreatedBeforePush:
				{
					PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
					OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
					break;
				}
				case EAsyncPushWidgetState::AfterPush:
				{
					AfterPush.Broadcast(PushedWidget);

					if(bCachedFocusOnNewlyPushedWidget)
					{
						if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
						{
							WidgetToFocus->SetFocus();
						}
					}
					SetReadyToDestroy();
					
					break;
				}
			}
		}
	);
}
