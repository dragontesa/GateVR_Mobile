// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/CConsoleCommandManager.h"
#include "Blueprint/UserWidget.h"
#include "HAL/ConsoleManager.h"

UCConsoleCommandManager::UCConsoleCommandManager()
{
	RegistConsoleCommands();
}

UCConsoleCommandManager::~UCConsoleCommandManager()
{
}

void UCConsoleCommandManager::RegistConsoleCommands()
{
	FConsoleManager::Get().RegisterConsoleCommand(
		TEXT("DT"),
		TEXT("Toogle Debug Mode Tool"),
		FConsoleCommandDelegate::CreateLambda([this]()
			{
				UE_LOG(LogTemp, Log, TEXT("Toggle Debug Tool!"));

				UWorld* CurrentWorld = GEngine->GetWorldContexts()[0].World();
				if (CurrentWorld == nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Cannot get World!"));
					return;
				}

				TArray<APlayerController*> ArrayPCL;
				GEngine->GetAllLocalPlayerControllers(ArrayPCL);

				if (ArrayPCL.IsEmpty() == false)
				{
					auto CurrentPlayerController = ArrayPCL[0];
					if (CurrentPlayerController)
					{
						const FName FuncName(TEXT("ToggleDebugTool"));
						auto ToggleFunction = CurrentPlayerController->FindFunctionChecked(FuncName);
						if (ToggleFunction)
						{
							// Call to Function in BP
							CurrentPlayerController->ProcessEvent(ToggleFunction, nullptr);
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("ArrayPCL is empty!"));
				}

			}),
			ECVF_Default);
}