// // Copyright Yoan Rock 2023. All Rights Reserved.


#include "UIControlMachineWidget.h"

#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"
#include "Components/ListView.h"
#include "GameFramework/Character.h"
#include "IB_Test/Actors/MachineActor.h"
#include "IB_Test/Subsystems/RecipeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UUIControlMachineWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Combo)
	{
		// Bind the OnSelectionChanged event to the HandleSelectionChanged function
		Combo->OnSelectionChanged.AddDynamic(this, &UUIControlMachineWidget::HandleSelectionChanged);
	}

	const UWorld* World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::NativeConstruct - World is invalid"));
		return;
	}
	
	RecipeSubsystem = World->GetSubsystem<URecipeSubsystem>();
	if(!ensure(RecipeSubsystem.IsValid()))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::NativeConstruct - RecipeSubsystem is invalid"));
		return;
	}
	
	// Retrieve Machine names
	TArray<FString> OutMachineNames = {};
	RecipeSubsystem->GetMachinesData().GetKeys(OutMachineNames);

	if(!ensure(OutMachineNames.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("UUIControlMachineWidget::NativeConstruct - it seems that no machines were placed in the level"));
		return;
	}

	for(const FString& MachineName : OutMachineNames)
	{
		Combo->AddOption(MachineName);
	}
	
	Combo->SetSelectedIndex(0);
}

void UUIControlMachineWidget::ToggleMachineWidget()
{	
	if(!ensure(Panel))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::ToggleMachineWidget - Panel is nullptr, probably because the Blueprint child wasn't linked"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!ensure(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::ToggleMachineWidget - PlayerController is invalid"));
		return;
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(!ensure(Character))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::ToggleMachineWidget - Character is invalid"));
		return;
	}
	
	if(IsVisible())
	{
		SetVisibility(ESlateVisibility::Hidden);

		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
		
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);

		const FInputModeGameAndUI InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
}

void UUIControlMachineWidget::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const UWorld* World = GetWorld();
	if(!ensure(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::HandleSelectionChanged - World is invalid"));
		return;
	}
	
	const TSoftObjectPtr<AMachineActor> SelectedMachineFound = RecipeSubsystem->GetMachineActorByName(SelectedItem);
	if(!ensure(SelectedMachineFound.IsValid()))
	{
		UE_LOG(LogTemp, Error, TEXT("UUIControlMachineWidget::HandleSelectionChanged - the SelectedMachineFound is invalid"));
		return;
	}

	RecipeSubsystem->SetSelectedMachine(SelectedMachineFound);
	
	ListView->SetListItems(SelectedMachineFound->GetRecipeEntries());
}
