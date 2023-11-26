// Copyright Yoan Rock 2023. All Rights Reserved.

#include "MachineActor.h"

#include "HelperClass.h"
#include "RecipeData.h"
#include "RecipeSubsystem.h"
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
		CachedRecipesData = RecipeSubsystem->GetRecipeDataByNames(AffectedRecipes);
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
	if(ensure(ShapeCollection))
	{
		TSoftObjectPtr<AShapeActor> ShapeToDestroy = ShapeCollection->Shapes.Pop();
		return ShapeToDestroy->Destroy();
	}

	return false;
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

bool AMachineActor::SpawnShapeByName(const FName& ShapeName)
{
	if(!RecipeSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - Invalid RecipeSubsystem"));
		return false;
	}
	const TSubclassOf<AShapeActor> ShapeClass = RecipeSubsystem->GetShapeActorClassByName(ShapeName);
	if(!ShapeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - Invalid ShapeClass associated with Shape Name : %s"), *(ShapeName.ToString()));
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
	UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - World is nullptr. Failed to spawn shape."));
		return false;
	}
	const AActor* SpawnedActor = World->SpawnActor<AActor>(ShapeClass, GetActorLocation(), FRotator(), SpawnParameters);
	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - Spawning Shape %s using class %s failed"), *(ShapeName.ToString()), *ShapeClass.Get()->GetName());
		return false;
	}
	
	return true;
}

void AMachineActor::ProcessValidRecipes()
{
	for(const FRecipeData& Recipe : CachedRecipesData)
	{
		if(AreAllShapesInRecipe(Recipe))
		{
			DestroyShapesByName(UHelperClass::ConvertToNames(Recipe.InputShape));
			
			SpawnShapeByName(UHelperClass::ConvertToName(Recipe.OutputShape));
		}
	}
}

bool AMachineActor::AreAllShapesInRecipe(const FRecipeData& RecipeData) const
{
	if(!ensure(RecipeData.InputShape.Num() > 0))
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::AreAllShapesInRecipe - No InputShape provided for recipe : %s"), *(RecipeData.Name.ToString()));
		return false;
	}
	
	for (const FText& ShapeName : RecipeData.InputShape)
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
	
	FShapeCollection* ShapeCollection = NearbyShapes.Find(Shape->GetShapeName());
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

	FShapeCollection* ShapeCollection = NearbyShapes.Find(Shape->GetShapeName());
	if(!ensure(ShapeCollection))
	{
		UE_LOG(LogTemp, Warning, TEXT("AMachineActor::OnColliderEndOverlap - Unknown shape : %s. It's likely that the shape was forgotten to be added in the data table."), *Shape->GetShapeName().ToString());
		return;
	}
	
	// Remove the previously Detected Shape in the NearbyShapes
	ShapeCollection->Shapes.RemoveSingle(Shape);
}
