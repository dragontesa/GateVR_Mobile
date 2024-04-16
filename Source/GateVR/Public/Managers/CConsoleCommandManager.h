// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CConsoleCommandManager.generated.h"

/**
 * Console Command Manager
 */
UCLASS()
class GATEVR_API UCConsoleCommandManager : public UObject
{
	GENERATED_BODY()

public:
	UCConsoleCommandManager();
	~UCConsoleCommandManager();

	void RegistConsoleCommands();
};
