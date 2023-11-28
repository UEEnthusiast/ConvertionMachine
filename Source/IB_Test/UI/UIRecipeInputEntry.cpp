// Copyright Yoan Rock 2023. All Rights Reserved.


#include "UIRecipeInputEntry.h"
#include "RecipeInputEntry.h"
#include "Components/TextBlock.h"

void UUIRecipeInputEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUIRecipeInputEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const URecipeInputItem* Item = Cast<URecipeInputItem>(ListItemObject);
	InputName->SetText(Item->InputName);
}