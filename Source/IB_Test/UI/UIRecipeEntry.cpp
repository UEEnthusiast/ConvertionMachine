// Copyright Yoan Rock 2023. All Rights Reserved.


#include "UIRecipeEntry.h"
#include "RecipeDataEntry.h"
#include "RecipeInputEntry.h"
#include "UIRecipeInputEntry.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "IB_Test/Subsystems/RecipeSubsystem.h"

void UUIRecipeEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
	CheckBox->OnCheckStateChanged.AddDynamic(this, &UUIRecipeEntry::OnCheckStateChanged);
	SpawnRecipe->OnClicked.AddDynamic(this, &UUIRecipeEntry::OnClickedRecipeSpawned);
}

void UUIRecipeEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const URecipeDataItem* Item = Cast<URecipeDataItem>(ListItemObject);

	RecipeName = Item->Name;
	NameLabel->SetText(Item->Name);

	SetInputList(Item);
	
	Output->SetText(Item->OutputShape);
	CheckBox->SetIsChecked(Item->bIsActivated);
}

void UUIRecipeEntry::SetInputList(const URecipeDataItem* Item)
{
	TArray<URecipeInputItem*> InputItems = {};

	// Iterate over input names in the recipe data item
	for(const FText& InputName : Item->InputNames)
	{
		// Create a new URecipeInputItem and initialize it with the input name
		URecipeInputItem* RecipeInputItem = NewObject<URecipeInputItem>(this);
		RecipeInputItem->Initialize(InputName);
		
		// Add the created input item to the array
		InputItems.Add(RecipeInputItem);
	}
	
	// Set the list of input items in the input list view
	InputListView->SetListItems(InputItems);
}

void UUIRecipeEntry::OnClickedRecipeSpawned()
{
	const UWorld* World = GetWorld();
	if(!ensure(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIRecipeEntry::OnClickedRecipeSpawned - World is invalid"));
		return;
	}
	
	World->GetSubsystem<URecipeSubsystem>()->OnRecipeEntryClicked.Broadcast(RecipeName);
}

void UUIRecipeEntry::OnCheckStateChanged(bool bIsChecked)
{
	const UWorld* World = GetWorld();
	if(!ensure(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIRecipeEntry::OnCheckStateChanged - World is invalid"));
		return;
	}
	
	World->GetSubsystem<URecipeSubsystem>()->OnToggleRecipeAvailability.Broadcast(RecipeName, bIsChecked);
}
