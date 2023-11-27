// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IB_Test/Actors/MachineActor.h"
#include "Subsystems/WorldSubsystem.h"
#include "RecipeSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnRecipe, FText, RecipeName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToggleRecipeAvailability, FText, RecipeName, bool, bIsActivated);

class AMachineActor;
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
	
	// Delegate used to send event to spawn a recipe to the selected machine
	FOnSpawnRecipe OnRecipeEntryClicked;

	// Delegate used to send enable/disable any recipe for the selected machine
	FOnToggleRecipeAvailability OnToggleRecipeAvailability;
	
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

	const TMap<FString, AMachineActor*>& GetMachinesData() const
	{
		return Machines;
	}

	TSoftObjectPtr<AMachineActor> GetSelectedMachine() const
	{
		if(!ensure(SelectedMachine.IsValid()))
		{
			//Todo : LOG
			return nullptr;
		}
		return SelectedMachine;
	}

	void SetSelectedMachine(const TSoftObjectPtr<AMachineActor>& InSelectedMachine)
	{
		SelectedMachine = InSelectedMachine;
	}

	TSoftObjectPtr<AMachineActor> GetMachineActorByName(const FString& MachineName) const;

	/**
	 * Spawn a shape by its name.
	 *
	 * @param ShapeName The name of the shape to be spawned.
	 * @return True if the shape was successfully spawned, false otherwise.
	 */
	bool SpawnShapeByName(const FName& ShapeName, AMachineActor* MachineActor) const;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION()
	void OnRecipeSpawn(FText RecipeName);

	UFUNCTION()
	void OnToggleRecipe(FText RecipeName, bool bIsActivated);

private:

	// Cached data table for recipes.
	UPROPERTY()
	TSoftObjectPtr<UDataTable> CachedRecipeDataTable;
	
	// Cached data table for shapes.
	UPROPERTY()
	TSoftObjectPtr<UDataTable> CachedShapeDataTable;

	UPROPERTY(Transient)
	TMap<FString, AMachineActor*> Machines = {};

	/*
	 * Cached value of the machine selected in the UI
	 */
	UPROPERTY(Transient)
	TSoftObjectPtr<AMachineActor> SelectedMachine = nullptr;
};
