// Fill out your copyright notice in the Description page of Project Settings.


#include "GateGameLiftServerWrapper.h"
#include "GateGameLiftGameMode.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#endif

bool UGateGameLiftServerWrapper::RemovePlayerSession(const FString& playerSessionId)
{
#if WITH_GAMELIFT
    FGameLiftServerSDKModule *gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
    auto outcome = gameLiftSdkModule->RemovePlayerSession(playerSessionId);
    if (outcome.IsSuccess()){
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Success to remove GameLift Player Session: psses=%s"), *playerSessionId);
        return true;
    }
    else {
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Failed to remove GameLift Player Session: psses=%s - %s"), *playerSessionId, *(outcome.GetError().m_errorMessage));
        return false;
    }
#endif
  return false;
}

bool UGateGameLiftServerWrapper::FinishGameSession()
{
#if WITH_GAMELIFT
    FGameLiftServerSDKModule *gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
    auto outcome =  gameLiftSdkModule->ProcessEnding();
    if (outcome.IsSuccess()) {
        // output log
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Game Session Finished:%s"), *(gameLiftSdkModule->GetGameSessionId().GetResult()));
        return true;
    }
    else {
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Failed to finish Game Session:%s"), *(outcome.GetError().m_errorMessage));
        return false;
    }
#endif
    return false;
}