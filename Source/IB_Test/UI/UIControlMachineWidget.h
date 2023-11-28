// // Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIControlMachineWidget.generated.h"

class URecipeSubsystem;
class AMachineActor;

/**
 * This widget provides functionality for controlling machines
 */
UCLASS()
class IB_TEST_API UUIControlMachineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Blueprint callable function to toggle the visibility of the machine widget.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleMachineWidget();

	/**
	 * @brief Handles the selection change event.
	 * 
	 * This function is bound to a selection change event and is called when the user makes a selection.
	 *
	 * @param SelectedItem The string representing the selected item.
	 * @param SelectionType The type of selection event that occurred.
	 */
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Panel = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* Combo = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UListView* ListView = nullptr;

	/*
	* Recipe subsystem simply stored to be easily accessed
	*/
	UPROPERTY(Transient)
	TSoftObjectPtr<URecipeSubsystem> RecipeSubsystem;
};
