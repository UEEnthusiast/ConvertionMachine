// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RecipeDataEntry.generated.h"

/**
 * Object containing all necessary information for a Recipe in the UI.
 */
UCLASS()
class URecipeDataItem : public UObject
{
	GENERATED_BODY()
	
public:
	URecipeDataItem() = default;

	void Initialize(const FText& InName,const TArray<FText>& InInputShape, const FText& InOutputShape)
	{
		Name = InName;
		InputNames = InInputShape;
		OutputShape = InOutputShape;
		bIsActivated = true;
	}

	/**
	 * Name of the Recipe
	 */
	UPROPERTY(EditAnywhere)
	FText Name = FText();
	
	/**
	 * Shapes that need to be ingested for this recipe.
	 */
	UPROPERTY(EditAnywhere)
	TArray<FText> InputNames = {};

	/**
	 * Shapes produced by the recipe.
	 */
	UPROPERTY(EditAnywhere)
	FText OutputShape = FText();

	/**
	 * Recipe's state
	 */
	UPROPERTY(EditAnywhere)
	bool bIsActivated = true;
};