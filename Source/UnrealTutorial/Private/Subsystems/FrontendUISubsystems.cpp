// Sinha Productions



#include "Subsystems/FrontendUISubsystems.h"
#include "FrontendDebugHelper.h"
#include "Engine/AssetManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "GameplayTagContainer.h"
#include "FrontedGameplayTags.h"
#include "FrontendFunctionLibrary.h"
#include "Widgets/Widget_ConfirmSCreen.h"
#include "Widgets/Widget_PrimaryLayout.h"

// Retrieve the singleton instance of this subsystem from any UObject context.
// - WorldContextObject: any UObject with a world (e.g., Actor, GameInstance, etc.)
// Returns nullptr if GEngine is unavailable or subsystem cannot be found.
UFrontendUISubsystems* UFrontendUISubsystems::Get(const UObject* WorldContextObject) {
    if (GEngine) {
        // Resolve UWorld from the provided context object (assert on failure).
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

        // Retrieve the subsystem instance from the GameInstance owning the world.
        return UGameInstance::GetSubsystem<UFrontendUISubsystems>(World->GetGameInstance());
    }

    // Engine is not available, can't resolve subsystem.
    return nullptr;
}

// Determine whether this subsystem should be created for the provided Outer.
// - Outer is expected to be a UGameInstance.
// Behavior:
// - Do not create on dedicated server instances (UI is client-side only).
// - Only create the base subsystem class if there are no derived classes present
//   to avoid creating both base and derived subsystem instances.
bool UFrontendUISubsystems::ShouldCreateSubsystem(UObject* Outer) const
{
    // Ensure the outer is a UGameInstance; will assert on incorrect type.
    if(!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
    {
        // Gather any classes that are derived from this class.
        // If there are derived classes, avoid creating this base class instance.
        TArray<UClass*> FoundClasses;
        GetDerivedClasses(GetClass(), FoundClasses);
        return FoundClasses.IsEmpty();
    }
     
    // Outer is a dedicated server: do not create UI subsystem.
    return false;
}

// Register the primary layout widget created by the UI flow.
// - InCreatedWidget: the widget instance to store for later operations.
// Side-effects:
// - Stores the pointer in CreatedPrimaryLayout and logs a debug message.
void UFrontendUISubsystems::RegisterCreatedPrimaryLayout(UWidget_PrimaryLayout* InCreatedWidget)
{
    // Ensure the incoming widget is valid.
    check(InCreatedWidget);

    // Cache the primary layout for subsequent widget stack operations.
    CreatedPrimaryLayout = InCreatedWidget;

    // Helpful debug output for development.
    Debug::Print(TEXT("Primary Widget Stored"));
}

// Asynchronously load a soft-referenced activatable widget class and push an instance
// into the widget stack identified by InWidgetStackTag.
// - InWidgetStackTag: gameplay tag identifying which widget stack to use.
// - InSoftWidgetClass: soft class pointer to the UWidget_ActivatableBase derived widget.
// - AsyncPushStateCallback: callback invoked with state changes and the created widget.
//
// Notes on state callbacks:
// - EAsyncPushWidgetState::OnCreatedBeforePush: invoked during instance initialization callback.
// - EAsyncPushWidgetState::AfterPush: invoked after the widget has been added to the stack.
void UFrontendUISubsystems::PushSoftWidgetToStackAsync(
    const FGameplayTag& InWidgetStackTag, 
    TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, 
    TFunction<void(EAsyncPushWidgetState, 
        UWidget_ActivatableBase*)> AsyncPushStateCallback)
{
    // Ensure the soft class reference is valid (not null).
    check(!InSoftWidgetClass.IsNull());

    // Request async loading of the soft class asset via the global AssetManager.
    UAssetManager::GetStreamableManager().RequestAsyncLoad(
        InSoftWidgetClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda(

            // Lambda executed once the soft class asset is loaded.
            // Captures:
            // - InSoftWidgetClass: to resolve the loaded UClass.
            // - this: to access CreatedPrimaryLayout and subsystem state.
            // - InWidgetStackTag: tag used to find the correct container stack.
            // - AsyncPushStateCallback: to notify caller about progress.
            [InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]()
            {
                // Resolve the loaded UClass from the soft pointer.
                UClass* LoadedWidgetClass = InSoftWidgetClass.Get();

                // Ensure the class was loaded successfully and that the primary layout exists.
                check(LoadedWidgetClass && CreatedPrimaryLayout);

                // Find the activatable widget stack inside the primary layout by tag.
                UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);

                // Add the widget instance to the found stack. Provide an instance init lambda
                // that forwards an 'OnCreatedBeforePush' state to the caller.
                UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>
                    (
                        LoadedWidgetClass,

                        // Instance initialization callback invoked once the widget instance is constructed
                        // but before it is fully pushed/activated. Forward this event to the caller.
                        [AsyncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
                        {
                            AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
                        }

                    );

                // Notify caller that the widget has been pushed/added to the stack.
                AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);

            }

        )

    );

}

void UFrontendUISubsystems::PushConfirmScreenToModalAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScrenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
    UConfirmScreenInfoObject* CreatedInfoObject = nullptr;

    switch (InScreenType)
    {

    case EConfirmScreenType::Okay:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScrenMessage);
        break;

    case EConfirmScreenType::YesNo:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScrenMessage);
        break;

    case EConfirmScreenType::OKCancel:
        CreatedInfoObject = UConfirmScreenInfoObject::CreateCancelScreen(InScreenTitle, InScrenMessage);
        break;

    case EConfirmScreenType::Unknown:
        break;

    default:
        break;

    }

    check(CreatedInfoObject);

    PushSoftWidgetToStackAsync(

        FrontendGameplayTags::Frontend_WidgetStack_Modal,
        UFrontendFunctionLibrary::GetFrontednSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widgets_ConfirmScreen),
        [CreatedInfoObject,ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
        {
            if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
            {
                UWidget_ConfirmScreen* CreatedCastChecked = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
                CreatedCastChecked->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
            }
        }

    );

}
