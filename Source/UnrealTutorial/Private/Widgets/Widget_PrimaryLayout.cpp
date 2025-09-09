// Sinha Productions


#include "Widgets/Widget_PrimaryLayout.h"
#include "FrontendDebugHelper.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag InTag) const
{

	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("Cannnot find the widget stack by the tag"), *InTag.ToString());


	return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{

	if (!IsDesignTime()) {

		if(!RegisteredWidgetStackMap.Contains(InStackTag)){

			RegisteredWidgetStackMap.Add(InStackTag, InStack);

		}

	}

}
