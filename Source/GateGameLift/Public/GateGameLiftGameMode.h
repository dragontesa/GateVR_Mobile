// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GateGameLiftGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GateGameLiftServerLog, Log, All);
/**
 * 
 */
UCLASS()
class GATEGAMELIFT_API AGateGameLiftGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGateGameLiftGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void InitGameLiftSDK();
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	bool IsAnywhere;
	bool TurnOffGameLift;
	uint16 ListenPort;
	// Params to pass GameLift SDK  if Server run on Anyhwere fleet
	FString FleetId; // this is used only for Anywhere fleet
	FString HostName;
	FString AuthToken;
	FString WebSocketUrl;


	static bool IsGameLiftServerInited;
};
