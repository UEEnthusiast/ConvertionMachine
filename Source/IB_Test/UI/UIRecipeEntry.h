// // Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RecipeDataEntry.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UIRecipeEntry.generated.h"

/**
 * Represents a user interface entry for a recipe.
 * This abstract class serves as the base for specific recipe entry widgets.
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

	/**
	 * Broadcasts a click event to notify the RecipeSubsystem to spawn the recipe's output.
	 */
	UFUNCTION()
	void OnClickedRecipeSpawned();

	/**
	 * Broadcasts a state change event to notify the RecipeSubsystem of a change in recipe availability.
	 *
	 * @param bIsChecked - The new state of the recipe availability checkbox.
	 */
	UFUNCTION()
	void OnCheckStateChanged(bool bIsChecked);

private:
	/**
	 * This function creates URecipeInputItem objects for each input name in the recipe data item
	 * and populates the input list view with these items.
	 *
	 * @param Item The recipe data item containing input names.
	 */
	void SetInputList(const URecipeDataItem* Item);

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameLabel = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UListView* InputListView = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Output = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UButton* SpawnRecipe = nullptr;

	UPROPERTY(Transient)
	FText RecipeName = {};
};
