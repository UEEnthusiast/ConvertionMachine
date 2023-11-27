// // Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIControlMachineWidget.generated.h"

class URecipeSubsystem;
class AMachineActor;

/**
 * Widget used for Controlling the Machines
 */
UCLASS()
class IB_TEST_API UUIControlMachineWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void ToggleMachineWidget();

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
