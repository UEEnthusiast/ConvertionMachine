// // Copyright Yoan Rock 2023. All Rights Reserved.


#include "UIControlMachineWidget.h"

#include "Components/CanvasPanel.h"
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
		//Todo: LOG
		return;
	}
	
	RecipeSubsystem = World->GetSubsystem<URecipeSubsystem>();
	if(!ensure(RecipeSubsystem.IsValid()))
	{
		//Todo: LOG
		return;
	}
	
	// Retrieve Machine names
	TArray<FString> OutMachineNames = {};
	RecipeSubsystem->GetMachinesData().GetKeys(OutMachineNames);

	if(!ensure(OutMachineNames.Num() > 0))
	{
		//Todo: LOG
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
		//Todo LOG
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!ensure(PlayerController))
	{
		//Todo LOG
		return;
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(!ensure(Character))
	{
		//Todo LOG
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
		//Todo: LOG
		return;
	}
	
	const TSoftObjectPtr<AMachineActor> SelectedMachineFound = RecipeSubsystem->GetMachineActorByName(SelectedItem);
	if(!ensure(SelectedMachineFound.IsValid()))
	{
		//Todo: LOG
		return;
	}

	RecipeSubsystem->SetSelectedMachine(SelectedMachineFound);
	
	ListView->SetListItems(SelectedMachineFound->GetRecipeEntries());
}
