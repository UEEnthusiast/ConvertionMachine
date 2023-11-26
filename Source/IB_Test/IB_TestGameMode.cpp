// Copyright Epic Games, Inc. All Rights Reserved.

#include "IB_TestGameMode.h"
#include "IB_TestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIB_TestGameMode::AIB_TestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
