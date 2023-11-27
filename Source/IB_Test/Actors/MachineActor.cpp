// Copyright Yoan Rock 2023. All Rights Reserved.

#include "MachineActor.h"

#include "IB_Test/Utilities/HelperClass.h"
#include "IB_Test/Datas/RecipeData.h"
#include "IB_Test/Subsystems/RecipeSubsystem.h"
#include "ShapeActor.h"

AMachineActor::AMachineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MachineMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MachineMesh"));
	SetRootComponent(MachineMesh);

	Collider = CreateDefaultSubobject<USphereComponent>(FName("Collider"));
	Collider->SetupAttachment(MachineMesh);
	Collider->InitSphereRadius(200.f);	
}

TArray<URecipeDataItem*> AMachineActor::GetRecipeEntries() const
{
	TArray<URecipeDataItem*> RecipeEntries = {};
	for(const TPair<FName, URecipeDataItem*> Pair : RecipeDataEntries)
	{
		RecipeEntries.AddUnique(Pair.Value);
	}
	return RecipeEntries;
}

bool AMachineActor::SetRecipeAvailability(const FText& RecipeName, bool bIsActivated)
{
	URecipeDataItem* RecipeDataEntry = RecipeDataEntries.FindChecked(UHelperClass::ConvertToName(RecipeName));
	if(!ensure(RecipeDataEntry))
	{
		//Todo : LOG
		return false;
	}
	
	RecipeDataEntry->bIsActivated = bIsActivated;
	return true;
}

void AMachineActor::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AMachineActor::OnColliderBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AMachineActor::OnColliderEndOverlap);
	
	const UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::BeginPlay - World is nullptr. Failed to proceed in BeginPlay"));
		return;
	}
	if(AffectedRecipes.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMachineActor::BeginPlay - No recipes provided in AffectedRecipes for Machine %s"), *GetName());
		return;
	}

	// Cache the Recipe Subsystem instance for easy access.
	RecipeSubsystem = World->GetSubsystem<URecipeSubsystem>();
	if(RecipeSubsystem.IsValid())
	{
		// Cache RecipeData for the Recipes associated with this machine.
		TArray<FRecipeData> RecipesData = RecipeSubsystem->GetRecipeDataByNames(AffectedRecipes);

		for(const FRecipeData RecipeData : RecipesData)
		{
			URecipeDataItem* RecipeItem = NewObject<URecipeDataItem>(this);
			RecipeItem->Initialize(RecipeData.Name, RecipeData.InputShape, RecipeData.OutputShape);
			
			RecipeDataEntries.Add(UHelperClass::ConvertToName(RecipeData.Name), RecipeItem);
		}
	}

	// Populate the NearbyShapes array with keys for each possible Shape.
	for(const FName& ShapeName : RecipeSubsystem->GetAllShapeNames())
	{
		NearbyShapes.Add(ShapeName);
	}
}

bool AMachineActor::DestroyShapeByName(const FName& ShapeName)
{
	FShapeCollection* ShapeCollection = NearbyShapes.Find(ShapeName);
	if(!ensure(ShapeCollection))
	{
		return false;
	}

	TSoftObjectPtr<AShapeActor> ShapeToDestroy = ShapeCollection->Shapes.Pop();
	return ShapeToDestroy->Destroy();

}

bool AMachineActor::DestroyShapesByName(const TArray<FName>& ShapeNames)
{
	bool ShapeAllDestroyed = true;
	for(const FName& ShapeName : ShapeNames)
	{
		ShapeAllDestroyed &= DestroyShapeByName(ShapeName);
	}

	return ShapeAllDestroyed;
}

void AMachineActor::ProcessValidRecipes()
{
	for(const TPair<FName, URecipeDataItem*> Pair : RecipeDataEntries)
	{
		const URecipeDataItem& Recipe = *Pair.Value;
		if(AreAllShapesInRecipe(Recipe) && Recipe.bIsActivated)
		{
			DestroyShapesByName(UHelperClass::ConvertToNames(Recipe.InputShapes));
			
			RecipeSubsystem->SpawnShapeByName(UHelperClass::ConvertToName(Recipe.OutputShape), this);
		}
	}
}

bool AMachineActor::AreAllShapesInRecipe(const URecipeDataItem& RecipeData) const
{
	if(!ensure(RecipeData.InputShapes.Num() > 0))
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::AreAllShapesInRecipe - No InputShape provided for recipe : %s"), *(RecipeData.Name.ToString()));
		return false;
	}
	
	for (const FText& ShapeName : RecipeData.InputShapes)
	{
		const FShapeCollection* ShapeCollection = NearbyShapes.Find(UHelperClass::ConvertToName(ShapeName));
		if(!ensure(ShapeCollection))
		{
			UE_LOG(LogTemp, Error, TEXT("AMachineActor::AreAllShapesInRecipe - Unknown shape : %s"), *ShapeName.ToString());
			return false;
		}

		// We return as soon as we notice that we are missing a shape for the recipe
		if(ShapeCollection->Shapes.Num() == 0)
		{
			return false;
		}
	}

	return true;
}

void AMachineActor::OnColliderBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if(OtherActor == nullptr)
	{
		return;
	}
	
	const AShapeActor* Shape = Cast<AShapeActor>(OtherActor);
	if(!Shape)
	{
		return;
	}
	
	FShapeCollection* ShapeCollection = NearbyShapes.Find(UHelperClass::ConvertToName(Shape->GetShapeName()));
	if(!ensure(ShapeCollection))
	{
		UE_LOG(LogTemp, Warning, TEXT("AMachineActor::OnColliderBeginOverlap - Unknown shape : %s. It's likely that the shape was forgotten to be added in the data table."), *Shape->GetShapeName().ToString());
		return;
	}
	
	// Add the Detected Shape in the NearbyShapes
	ShapeCollection->Shapes.Add(Shape);
	
	// Look up if there is enough ingredients for a valid recipe
	ProcessValidRecipes();
}

void AMachineActor::OnColliderEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(OtherActor == nullptr)
	{
		return;
	}
	
	AShapeActor* Shape = Cast<AShapeActor>(OtherActor);
	if(!Shape)
	{
		return;
	}

	FShapeCollection* ShapeCollection = NearbyShapes.Find(UHelperClass::ConvertToName(Shape->GetShapeName()));
	if(!ensure(ShapeCollection))
	{
		UE_LOG(LogTemp, Warning, TEXT("AMachineActor::OnColliderEndOverlap - Unknown shape : %s. It's likely that the shape was forgotten to be added in the data table."), *Shape->GetShapeName().ToString());
		return;
	}
	
	// Remove the previously Detected Shape in the NearbyShapes
	ShapeCollection->Shapes.RemoveSingle(Shape);
}
