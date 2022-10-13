// Copyright Epic Games, Inc. All Rights Reserved.

#include "ALSWIthPIKGameMode.h"
#include "ALSWIthPIKCharacter.h"
#include "UObject/ConstructorHelpers.h"

AALSWIthPIKGameMode::AALSWIthPIKGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
