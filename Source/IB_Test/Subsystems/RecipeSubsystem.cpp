// Copyright Yoan Rock 2023. All Rights Reserved.


#include "RecipeSubsystem.h"

#include "EngineUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "IB_Test/Datas/RecipeData.h"
#include "IB_Test/Actors/MachineActor.h"
#include "IB_Test/Actors/ShapeActor.h"
#include "IB_Test/Datas/ShapeData.h"
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

	CachedRecipeDataTable = RecipeSettings->RecipeDataTable.LoadSynchronous();
	CachedShapeDataTable = RecipeSettings->ShapeDataTable.LoadSynchronous();
	CachedSpawnVfx = RecipeSettings->SpawnVfx.LoadSynchronous();
}

void URecipeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	for (TActorIterator<AMachineActor> ActorItr(&InWorld); ActorItr; ++ActorItr)
	{
		AMachineActor* Machine = *ActorItr;
		if (Machine)
		{
			Machines.Add(Machine->GetMachineName(), Machine);
		}
	}

	if(!ensure(Machines.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("URecipeSubsystem::OnWorldBeginPlay - No Machine were placed in the world"));
		return;
	}

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

TArray<FRecipeData> URecipeSubsystem::GetRecipeDataByNames(const TArray<FText>& RecipeNames) const
{
	TArray<FRecipeData> Recipes = {};
	for(const FText Name : RecipeNames)
	{
		Recipes.Add(GetRecipeDataByName(Name));
	}
	
	return Recipes;
}

FRecipeData URecipeSubsystem::GetRecipeDataByName(const FText& InRecipeName) const
{
	static const FString ContextString(TEXT("DataTableContext"));
	const FName RecipeName = UHelperClass::ConvertToName(InRecipeName);
	
	FRecipeData* FoundRow = CachedRecipeDataTable->FindRow<FRecipeData>(RecipeName, ContextString);
	if(FoundRow)
	{
		return *FoundRow;
	}
	
	checkNoEntry()
	return {};
}

TSubclassOf<AShapeActor> URecipeSubsystem::GetShapeActorClassByName(const FName& InShapeName) const
{
	static const FString ContextString(TEXT("DataTableContext"));
	
	FShapeData* FoundShapeData = CachedShapeDataTable->FindRow<FShapeData>(InShapeName, ContextString);
	if(FoundShapeData)
	{
		return FoundShapeData->ShapeActorClass;
	}

	checkNoEntry();
	return {};
}

TArray<FName> URecipeSubsystem::GetAllShapeNames() const
{
	if(!ensure(CachedShapeDataTable.IsValid()))
	{
		return {};
	}
	
	return CachedShapeDataTable->GetRowNames();
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

bool URecipeSubsystem::SpawnShapeByName(const FName& ShapeName, AMachineActor& MachineActor) const
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
