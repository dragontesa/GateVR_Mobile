// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Auth/AWSCoreCredentials.h"
#include "Client/AWSClientConfiguration.h"
#include "Model/CreateGameSessionResult.h"
#include "Model/CreatePlayerSessionResult.h"
#include "GameLiftError.h"
#include "GateGameLiftClient.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(GateGameLiftClientLog, Log, All);

USTRUCT(BlueprintType)
struct FGameLiftClientData
{
	GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GateGameLift")
	FAWSCredentials awsCredentials;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GateGameLift")
	FAWSClientConfiguration awsConfiguration;
};


UCLASS()
class GATEGAMELIFT_API AGateGameLiftClient : public AActor
{
	GENERATED_BODY()
	
	/**
 * Out session id
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleGameSessionCreated, bool, success, FString, GameSessionId);
/**
 * Out PlayerSessionID, Ip Address, Port number
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHandlePlayerSessionCreated, bool, success, FString, PlayerSessionId, FString, IpAddress, int, PortNumber);

public:	
	// Sets default values for this actor's properties
	AGateGameLiftClient();
    
	UFUNCTION(BlueprintCallable, Category = "GateGameLift")
	void Init(const FGameLiftClientData& ClientReClientta);

	UFUNCTION(BlueprintCallable, Category = "GateGameLift")
	void CreateGameSession(FCreateGameSessionRequest request); 

    UFUNCTION(BlueprintCallable, Category = "GateGameLift")
	void CreatePlayerSession(FCreatePlayerSessionRequest request); 

	UFUNCTION(Category = "GateGameLift")
	void OnCreateGameSessionResult();

	UFUNCTION(Category = "GateGameLift")
	void OnCreatePlayerSessionResult();

	UFUNCTION(BlueprintCallable, Category = "GateGameLift")
	void SetisfyGameSession(const FString& sessionId);

	UPROPERTY(BlueprintAssignable, Category = "GateGameLift")
	FHandleGameSessionCreated OnGameSessionCreated;

    UPROPERTY(BlueprintAssignable, Category = "GateGameLift")
	FHandlePlayerSessionCreated OnPlayerSessionCreated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GateGameLift")
	FString FleetId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GateGameLift")
	FString GameSessionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = "GateGameLift")
	FString PlayerSessionId;

    // must be required to create gamelift client object
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class UGameLiftClientObject* gameLiftClientObj = nullptr;

private:
	FCreateGameSessionResult     createGameSessionResult;
	FCreatePlayerSessionResult   createPlayerSessionResult;
	bool isLastGameliftSuccess   = false;
	EGameLiftError     lastGameliftError;
	FString            lastGameliftErrorMessage;
};
