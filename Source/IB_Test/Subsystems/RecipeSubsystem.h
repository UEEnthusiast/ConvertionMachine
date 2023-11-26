// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RecipeSubsystem.generated.h"

class UDataTable;
class AShapeActor;
struct FRecipeData;

/**
 * Recipe and Shape Subsystem
 */
UCLASS()
class IB_TEST_API URecipeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Get an array of recipe data based on provided recipe names.
	 *
	 * @param RecipeNames The names of the recipes to retrieve.
	 * @return An array of recipe data.
	 */
	TArray<FRecipeData> GetRecipeDataByNames(const TArray<FText>& RecipeNames) const;

	/**
	 * Get recipe data based on a provided recipe name.
	 *
	 * @param RecipeName The name of the recipe to retrieve.
	 * @return The recipe data.
	 */
	FRecipeData GetRecipeDataByName(const FText& RecipeName) const;

	/**
	 * Get the shape actor class based on a provided shape name.
	 *
	 * @param ShapeName The name of the shape to retrieve.
	 * @return The class of the shape actor.
	 */
	TSubclassOf<AShapeActor> GetShapeActorClassByName(const FName& ShapeName) const;

	/**
	 * Get an array of all shape names.
	 *
	 * @return An array of shape names.
	 */
	TArray<FName> GetAllShapeNames() const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:

	// Cached data table for recipes.
	UPROPERTY()
	TSoftObjectPtr<UDataTable> CachedRecipeDataTable;
	
	// Cached data table for shapes.
	UPROPERTY()
	TSoftObjectPtr<UDataTable> CachedShapeDataTable;
};
