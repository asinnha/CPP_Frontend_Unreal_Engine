#pragma once

UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	Okay,
	YesNo,
	OKCancel,
	Unknown UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed,
	Canceled,
	Closed,
	Unknown UMETA(Hidden)
};