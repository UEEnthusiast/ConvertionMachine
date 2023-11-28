// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IB_Test/Actors/MachineActor.h"
#include "IB_Test/Settings/RecipeSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "IB_Test/Datas/ShapeData.h"
#include "IB_Test/Datas/RecipeData.h"
#include "RecipeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnRecipe, FText, RecipeName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToggleRecipeAvailability, FText, RecipeName, bool, bIsActivated);

class UNiagaraSystem;
class AMachineActor;
class UDataTable;
class AShapeActor;

/**
 * Subsystem responsible for managing recipes, shapes, and related functionalities within the game world.
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
	TArray<FRecipeData> GetRecipeDataByNames(const TArray<FText>& RecipeNames);

	/**
	 * Get recipe data based on a provided recipe name.
	 *
	 * @param RecipeName The name of the recipe to retrieve.
	 * @return The recipe data.
	 */
	FRecipeData GetRecipeDataByName(const FText& RecipeName);

	/**
	 * Get the shape actor class based on a provided shape name.
	 *
	 * @param ShapeName The name of the shape to retrieve.
	 * @return The class of the shape actor.
	 */
	TSubclassOf<AShapeActor> GetShapeActorClassByName(const FName& InShapeName);

	/**
	 * Get an array of all shape names.
	 *
	 * @return An array of shape names.
	 */
	TArray<FName> GetAllShapeNames() const;

	/**
	 * @brief Gets a const reference to the map of machine data.
	 *
	 * @return Const reference to the map containing machine data.
	 */
	const TMap<FString, AMachineActor*>& GetMachinesData() const
	{
		return Machines;
	}

	/**
	 * @brief Gets the currently selected machine in the UI
	 *
	 * @return Soft object pointer to the selected machine.
	 */
	TSoftObjectPtr<AMachineActor> GetSelectedMachine() const;

	/**
	 * @brief Sets the currently selected machine in the UI
	 *
	 * @param InSelectedMachine The soft object pointer to set as the selected machine.
	 */
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
	bool SpawnShapeByName(const FName& ShapeName, AMachineActor& MachineActor);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * @brief Initializes subsystem-specific details when the world begins play.
	 *
	 * This function is used to retrieve Machines in the world, not possible to do it in Initialize because it was too early.
	 *
	 * @param InWorld Reference to the world that has begun play.
	 */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/**
	 * @brief Callback function triggered when a recipe is spawned.
	 *
	 * @param RecipeName The name of the recipe to spawn.
	 */
	UFUNCTION()
	void OnRecipeSpawn(FText RecipeName);

	/**
	 * @brief Function triggered when a recipe's activation state is toggled.
	 *
	 * @param RecipeName The name of the recipe whose activation state is toggled.
	 * @param bIsActivated The new activation state of the recipe.
	 */
	UFUNCTION()
	void OnToggleRecipe(FText RecipeName, bool bIsActivated);

private:
	/**
	 * @brief Initializes the collection of machines in the specified world.
	 *
	 * @param InWorld The world in which to initialize the machine collection.
	 */
	void InitMachineCollection(UWorld& InWorld);

	/**
	 * @brief Sets up dynamic delegates for handling events related to machines and recipes.
	 *        Call this function during initialization to establish necessary event connections.
	 */
	void SetupDynamicDelegates();

	/**
	 * @brief Caches shape-related data from the provided recipe settings.
	 *
	 * @param RecipeSettings The settings containing shape-related data to be cached.
	 */
	void CacheShapeData(const URecipeSettings* RecipeSettings);

	/**
	 * @brief Caches recipe-related data from the provided recipe settings.
	 *
	 * @param RecipeSettings The settings containing recipe-related data to be cached.
	 */
	void CacheRecipeData(const URecipeSettings* RecipeSettings);

	/**
	 * @brief Caches visual effects (VFX) data from the provided recipe settings.
	 *
	 * @param RecipeSettings The settings containing VFX data to be cached.
	 */
	void CacheVfx(const URecipeSettings* RecipeSettings);

	/**
	 * @brief Spawns an output shape for a given machine (only used internally for now).
	 *
	 * @param ShapeClass The class of the shape to spawn.
	 * @param MachineActor Reference to the target machine.
	 * @return True if successful, false otherwise.
	 */
	bool SpawnOutputShape(const TSubclassOf<AShapeActor> ShapeClass, AMachineActor& MachineActor) const;
	
	/**
	 * @brief Spawns the cached visual effects at a specified location (only used internally for now).
	 *
	 * @param SpawnLocation The location at which to spawn the visual effects.
	 */
	void SpawnSpawnVfx(const FVector& SpawnLocation) const;
	
	/**
	 * @brief Collection of machines mapped by their name.
	 */
	UPROPERTY(Transient)
	TMap<FString, AMachineActor*> Machines = {};

	/**
	 *	Cached visual effects from settings, used to spawn a Niagara System when generating the output.
	 */
	UPROPERTY(Transient)
	UNiagaraSystem* CachedSpawnVfx = nullptr;

	/*
	 * Cached value of the machine selected in the UI
	 */
	UPROPERTY(Transient)
	TSoftObjectPtr<AMachineActor> SelectedMachine = nullptr;

	UPROPERTY(Transient)
	TMap<FName, FRecipeData> CachedRecipesData;

	UPROPERTY(Transient)
	TMap<FName, FShapeData> CachedShapesData;
};
