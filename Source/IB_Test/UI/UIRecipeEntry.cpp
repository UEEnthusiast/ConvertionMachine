// Copyright Yoan Rock 2023. All Rights Reserved.


#include "UIRecipeEntry.h"
#include "RecipeDataEntry.h"
#include "Components/Button.h"
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
	Input->SetText(FText::Join(FText::FromString(TEXT(", ")), Item->InputShapes));
	Output->SetText(Item->OutputShape);
	CheckBox->SetIsChecked(Item->bIsActivated);
}

void UUIRecipeEntry::OnClickedRecipeSpawned()
{
	const UWorld* World = GetWorld();
	if(!ensure(World))
	{
		//Todo LOG
		return;
	}
	
	World->GetSubsystem<URecipeSubsystem>()->OnRecipeEntryClicked.Broadcast(RecipeName);
}

void UUIRecipeEntry::OnCheckStateChanged(bool bIsChecked)
{
	const UWorld* World = GetWorld();
	if(!ensure(World))
	{
		//Todo LOG
		return;
	}
	
	World->GetSubsystem<URecipeSubsystem>()->OnToggleRecipeAvailability.Broadcast(RecipeName, bIsChecked);
}
