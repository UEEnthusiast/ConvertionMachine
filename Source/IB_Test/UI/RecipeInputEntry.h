// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RecipeInputEntry.generated.h"

/**
 * Object used by a ListView containing all necessary information for a recipe input in the UI.
 */
UCLASS()
class URecipeInputItem : public UObject
{
	GENERATED_BODY()
	
public:
	URecipeInputItem() = default;

	void Initialize(const FText& InInputName)
	{
		InputName = InInputName;
	}

	/**
	 * Name of the Input
	 */
	UPROPERTY(EditAnywhere)
	FText InputName = FText();

	/**
	 * Description of the Input
	 */
	UPROPERTY(EditAnywhere)
	FText InputDescription = FText();
};