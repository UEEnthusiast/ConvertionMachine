// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "IB_Test/UI/RecipeDataEntry.h"
#include "MachineActor.generated.h"

class URecipeSubsystem;
class AShapeActor;
struct FRecipeData;

/**
 * Simple Structure used as a value in our NearbyActor TMap since we can't have a direct TArray
 */
USTRUCT()
struct FShapeCollection
{
	GENERATED_BODY()
	
	FShapeCollection() = default;
	
	TArray<TSoftObjectPtr<AShapeActor>> Shapes;
};

/**
 * Actor representing a conversion machine
 */
UCLASS()
class IB_TEST_API AMachineActor : public AActor
{
	GENERATED_BODY()

public:
	AMachineActor();

	FString GetMachineName() const
	{
		return MachineName.ToString();
	}

	TArray<FText> GetAffectedRecipes() const
	{
		return AffectedRecipes;
	}
 
	TArray<URecipeDataItem*> GetRecipeEntries() const;

	bool SetRecipeAvailability(const FText& RecipeName, bool bIsActivated);

	/**
	 * Process all valid recipes for the machine, destroying input shapes and spawning output shapes as needed.
	 */
	void ProcessValidRecipes();

protected:
	virtual void BeginPlay() override;

	/**
	 * Destroy colliding shapes by their names.
	 *
	 * @param ShapeNames The array of shape names to be destroyed.
	 * @return True if all shapes were successfully destroyed, false otherwise.
	 */
	bool DestroyShapesByName(const TArray<FName>& ShapeNames);
	
	/**
	 * Destroy a shape by its name.
	 *
	 * @param ShapeName The name of the shape to be destroyed.
	 * @return True if the shape was successfully destroyed, false otherwise.
	 */
	bool DestroyShapeByName(const FName& ShapeName);

	/**
	 * Called when the collider begins to overlap with another actor
	 */
	UFUNCTION()
	void OnColliderBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/**
	 * Called when the collider ends overlap with another actor
	 */
	UFUNCTION()
	void OnColliderEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	/**
	 * Array of recipes that are affected by this machine.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Config")
	TArray<FText> AffectedRecipes;

	/**
	 * Mesh component representing the machine.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	UStaticMeshComponent* MachineMesh;

	/**
	 * The name of the shape.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Config")
	FText MachineName = FText();
	
private:

	/**
	 * Check if all required shapes for a recipe are present.
	 *
	 * @param RecipeData The recipe data to be checked.
	 * @return True if all required shapes are present, false otherwise.
	 */
	bool AreAllShapesInRecipe(const URecipeDataItem& RecipeData) const;
	
	/*
	* A simple TMap<FString,TSoftObjectPtr<AShapeActor>> isn't suitable in this context due to the potential presence of duplicate keys
	* So we need to use a TMap<FString, FShapeCollection>
	*/
	UPROPERTY(Transient)
	TMap<FName, FShapeCollection> NearbyShapes;

	/*
	* Recipe subsystem simply stored in BeginPlay() to be easily accessed
	*/
	UPROPERTY(Transient)
	TSoftObjectPtr<URecipeSubsystem> RecipeSubsystem;

	/*
	* Simple sphere collider used to detect nearby shapes
	*/
	UPROPERTY(Transient)
	TObjectPtr<USphereComponent> Collider;
	
	/*
	* Stores recipe data in BeginPlay to avoid reading the data table each time a recipe check is performed.
	* This improves performance by eliminating redundant data table lookups.
	* 
	* Data cache for UI-related recipe information
	*/
	UPROPERTY(Transient)
	TMap<FName, URecipeDataItem*> RecipeDataEntries = {};
};
