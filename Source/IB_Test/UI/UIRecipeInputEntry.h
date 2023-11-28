// // Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UIRecipeInputEntry.generated.h"

/**
 * Represents a user interface entry for a input shape.
 * This abstract class serves as the base for specific input shape entry widgets.
 */
UCLASS(Abstract)
class IB_TEST_API UUIRecipeInputEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override;

	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// IUserObjectListEntry

private:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* InputName = nullptr;
};
