// Copyright Yoan Rock 2023. All Rights Reserved.


#include "RecipeSubsystem.h"

#include "EngineUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "IB_Test/Actors/MachineActor.h"
#include "IB_Test/Actors/ShapeActor.h"
#include "IB_Test/Settings/RecipeSettings.h"
#include "Engine/DataTable.h"
#include "IB_Test/Utilities/HelperClass.h"

void URecipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const URecipeSettings* RecipeSettings = GetDefault<URecipeSettings>();
	if(!ensure(RecipeSettings))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::Initialize - RecipeSettings nullptr"));
		return;
	}
	
	CacheRecipeData(RecipeSettings);
	CacheShapeData(RecipeSettings);
	CacheVfx(RecipeSettings);
}

void URecipeSubsystem::CacheShapeData(const URecipeSettings* RecipeSettings)
{
	const TSoftObjectPtr<UDataTable> CachedShapeDataTable = RecipeSettings->ShapeDataTable.LoadSynchronous();
	if(!CachedShapeDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::Initialize - CachedShapeDataTable invalid"));
		return;
	}
	TArray<FShapeData*> OutShapesData = {};
	CachedShapeDataTable->GetAllRows<FShapeData>("",OutShapesData);

	if(!ensure(OutShapesData.Num() > 0))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::Initialize - OutShapesData empty, check the Shape DataTable"));
		return;
	}

	// Build our map to easily & efficiently retrieve data later
	for(const FShapeData* ShapeData : OutShapesData)
	{
		CachedShapesData.Add(UHelperClass::ConvertToName(ShapeData->Name), *ShapeData);
	}
}

void URecipeSubsystem::CacheRecipeData(const URecipeSettings* RecipeSettings)
{
	const TSoftObjectPtr<UDataTable> CachedRecipeDataTable = RecipeSettings->RecipeDataTable.LoadSynchronous();
	if(!CachedRecipeDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::Initialize - CachedRecipeDataTable invalid"));
	}
	TArray<FRecipeData*> OutRecipesData = {};
	CachedRecipeDataTable->GetAllRows<FRecipeData>("",OutRecipesData);

	if(!ensure(OutRecipesData.Num() > 0))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::Initialize - OutRecipesData empty, check the Recipe DataTable"));
	}

	// Build our map to easily & efficiently retrieve data later
	for(const FRecipeData* RecipeData : OutRecipesData)
	{
		CachedRecipesData.Add(UHelperClass::ConvertToName(RecipeData->Name), *RecipeData);
	}
}

void URecipeSubsystem::CacheVfx(const URecipeSettings* RecipeSettings)
{
	CachedSpawnVfx = RecipeSettings->SpawnVfx.LoadSynchronous();
}

void URecipeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InitMachineCollection(InWorld);
	SetupDynamicDelegates();
}

void URecipeSubsystem::InitMachineCollection(UWorld& InWorld)
{
	for (TActorIterator<AMachineActor> ActorItr(&InWorld); ActorItr; ++ActorItr)
	{
		AMachineActor* Machine = *ActorItr;
		if (Machine)
		{
			Machines.Add(Machine->GetMachineName(), Machine);
		}
	}

	ensure(Machines.Num() > 0);
}

void URecipeSubsystem::SetupDynamicDelegates()
{
	OnRecipeEntryClicked.AddDynamic(this, &URecipeSubsystem::OnRecipeSpawn);
	OnToggleRecipeAvailability.AddDynamic(this, &URecipeSubsystem::OnToggleRecipe);
}

void URecipeSubsystem::OnRecipeSpawn(FText RecipeName)
{
	const FRecipeData RecipeData = GetRecipeDataByName(RecipeName);
	
	SpawnShapeByName(UHelperClass::ConvertToName(RecipeData.OutputShape), *GetSelectedMachine().Get());
}

void URecipeSubsystem::OnToggleRecipe(FText RecipeName, bool bIsActivated)
{
	GetSelectedMachine()->SetRecipeAvailability(RecipeName, bIsActivated);

	GetSelectedMachine()->ProcessValidRecipes();
}

TArray<FRecipeData> URecipeSubsystem::GetRecipeDataByNames(const TArray<FText>& RecipeNames)
{
	TArray<FRecipeData> Recipes = {};
	for(const FText Name : RecipeNames)
	{
		Recipes.Add(GetRecipeDataByName(Name));
	}
	
	return Recipes;
}

FRecipeData URecipeSubsystem::GetRecipeDataByName(const FText& InRecipeName)
{
	return CachedRecipesData.FindChecked(UHelperClass::ConvertToName(InRecipeName));
}

TSubclassOf<AShapeActor> URecipeSubsystem::GetShapeActorClassByName(const FName& InShapeName)
{
	TSubclassOf<AShapeActor> ShapeActorClass = CachedShapesData.FindChecked(InShapeName).ShapeActorClass;
	if(!ensure(ShapeActorClass))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::GetShapeActorClassByName - ShapeActorClass invalid"));
		return {};
	}
	
	return ShapeActorClass;
}

TArray<FName> URecipeSubsystem::GetAllShapeNames() const
{
	TArray<FName> OutShapeNames = {};	
	CachedShapesData.GetKeys(OutShapeNames);

	return OutShapeNames;
}

TSoftObjectPtr<AMachineActor> URecipeSubsystem::GetSelectedMachine() const
{
	if(!ensure(SelectedMachine.IsValid()))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::GetSelectedMachine - SelectedMachine is invalid"));
		return nullptr;
	}
	return SelectedMachine;
}

TSoftObjectPtr<AMachineActor> URecipeSubsystem::GetMachineActorByName(const FString& MachineName) const
{
	const TSoftObjectPtr<AMachineActor> Machine = Machines.FindChecked(MachineName);
	if(!ensure(Machine.IsValid()))
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::GetMachineActorByName - World is nullptr. Failed to spawn shape."));
		return TSoftObjectPtr<AMachineActor>();
	}

	return Machine;
}

bool URecipeSubsystem::SpawnOutputShape(const TSubclassOf<AShapeActor> ShapeClass, AMachineActor& MachineActor) const
{
	UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::SpawnShape - World is nullptr. Failed to spawn shape."));
		return false;
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Cast<AActor>(&MachineActor);
	SpawnParameters.Instigator = MachineActor.GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	
	const AActor* SpawnedActor = World->SpawnActor<AActor>(ShapeClass, MachineActor.GetActorLocation(), FRotator(), SpawnParameters);
	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::SpawnShape - Spawning class %s failed"), *ShapeClass.Get()->GetName());
		return false;
	}

	return true;
}

bool URecipeSubsystem::SpawnShapeByName(const FName& ShapeName, AMachineActor& MachineActor)
{ 
	const TSubclassOf<AShapeActor> ShapeClass = GetShapeActorClassByName(ShapeName);
	if(!ShapeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::SpawnShapeByName - Invalid ShapeClass associated with Shape Name : %s"), *(ShapeName.ToString()));
		return false;
	}
	
	const bool IsSpawned = SpawnOutputShape(ShapeClass, MachineActor);
	if(IsSpawned)
	{
		SpawnSpawnVfx(MachineActor.GetActorLocation());
	}
	return IsSpawned;
}

void URecipeSubsystem::SpawnSpawnVfx(const FVector& SpawnLocation) const
{
	UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("URecipeSubsystem::SpawnSpawnVfx - World is nullptr. Failed to spawn shape."));
		return;
	}
	
	// Spawn VFX
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, CachedSpawnVfx, SpawnLocation);
}
