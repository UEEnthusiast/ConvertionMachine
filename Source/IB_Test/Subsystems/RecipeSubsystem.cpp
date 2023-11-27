// Copyright Yoan Rock 2023. All Rights Reserved.


#include "RecipeSubsystem.h"

#include "EngineUtils.h"
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
		//Todo : LOG
		return;
	}

	CachedRecipeDataTable = RecipeSettings->RecipeDataTable.LoadSynchronous();
	CachedShapeDataTable = RecipeSettings->ShapeDataTable.LoadSynchronous();
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
		//Todo: Log
		return;
	}

	OnRecipeEntryClicked.AddDynamic(this, &URecipeSubsystem::OnRecipeSpawn);
	OnToggleRecipeAvailability.AddDynamic(this, &URecipeSubsystem::OnToggleRecipe);
}

void URecipeSubsystem::OnRecipeSpawn(FText RecipeName)
{
	const FRecipeData RecipeData = GetRecipeDataByName(RecipeName);
	
	SpawnShapeByName(UHelperClass::ConvertToName(RecipeData.OutputShape), GetSelectedMachine().Get());
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

	checkNoEntry()
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

TSoftObjectPtr<AMachineActor> URecipeSubsystem::GetMachineActorByName(const FString& MachineName) const
{
	const TSoftObjectPtr<AMachineActor> Machine = Machines.FindChecked(MachineName);
	if(!ensure(Machine.IsValid()))
	{
		//Todo LOG
		return TSoftObjectPtr<AMachineActor>();
	}

	return Machine;
}

bool URecipeSubsystem::SpawnShapeByName(const FName& ShapeName, AMachineActor* MachineActor) const
{ 
	const TSubclassOf<AShapeActor> ShapeClass = GetShapeActorClassByName(ShapeName);
	if(!ShapeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - Invalid ShapeClass associated with Shape Name : %s"), *(ShapeName.ToString()));
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Cast<AActor>(MachineActor);
	SpawnParameters.Instigator = MachineActor->GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
	UWorld* World = GetWorld();
	if(!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - World is nullptr. Failed to spawn shape."));
		return false;
	}
	const AActor* SpawnedActor = World->SpawnActor<AActor>(ShapeClass, MachineActor->GetActorLocation(), FRotator(), SpawnParameters);
	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("AMachineActor::SpawnShapeByName - Spawning Shape %s using class %s failed"), *(ShapeName.ToString()), *ShapeClass.Get()->GetName());
		return false;
	}
	
	return true;
}
