// Copyright Yoan Rock 2023. All Rights Reserved.


#include "RecipeSubsystem.h"
#include "RecipeData.h"
#include "ShapeData.h"
#include "RecipeSettings.h"
#include "ShapeActor.h"
#include "Engine/DataTable.h"

void URecipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const URecipeSettings* RecipeSettings = GetDefault<URecipeSettings>();
	if(ensure(RecipeSettings))
	{
		CachedRecipeDataTable = RecipeSettings->RecipeDataTable.LoadSynchronous();
		CachedShapeDataTable = RecipeSettings->ShapeDataTable.LoadSynchronous();
	}
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
	const FName RecipeName = FName(InRecipeName.ToString());
	
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
