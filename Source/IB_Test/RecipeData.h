// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RecipeData.generated.h"

/**
 * Data structure containing all necessary information for a Recipe.
 */
USTRUCT(BlueprintType)
struct IB_TEST_API FRecipeData : public FTableRowBase
{
	GENERATED_BODY()

	FRecipeData() = default;

	FRecipeData(const FText& InName,const TArray<FText>& InInputShape, const FText& InOutputShape) :
	Name(InName), InputShape(InInputShape), OutputShape(InOutputShape)
	{
	}

	/**
	 * Name of the Recipe
	 */
	UPROPERTY(EditAnywhere)
	FText Name;
	
	/**
	 * Shapes that need to be ingested for this recipe.
	 */
	UPROPERTY(EditAnywhere)
	TArray<FText> InputShape;

	/**
	 * Shapes produced by the recipe.
	 */
	UPROPERTY(EditAnywhere)
	FText OutputShape;
};