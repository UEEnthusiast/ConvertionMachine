// // Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UIRecipeEntry.generated.h"

/**
 * Entry for a Shape
 */
UCLASS(Abstract)
class IB_TEST_API UUIRecipeEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override;

	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// IUserObjectListEntry

	UFUNCTION()
	void OnClickedRecipeSpawned();

	UFUNCTION()
	void OnCheckStateChanged(bool bIsChecked);

private:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameLabel = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Input = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Output = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UButton* SpawnRecipe = nullptr;

	UPROPERTY(Transient)
	FText RecipeName = {};
};
