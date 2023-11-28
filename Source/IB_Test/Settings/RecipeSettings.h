// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RecipeSettings.generated.h"

class UNiagaraSystem;
class UDataTable;
/**
 * Custom class settings for recipe-related configurations.
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Recipe Settings"))
class IB_TEST_API URecipeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/* DataTable of recipes */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> RecipeDataTable;

	/* DataTable of shapes */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> ShapeDataTable;

	/* VFX used when spawning the recipe output*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VFX", AdvancedDisplay)
	TSoftObjectPtr<UNiagaraSystem> SpawnVfx;
};
