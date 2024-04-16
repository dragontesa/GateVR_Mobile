// Fill out your copyright notice in the Description page of Project Settings.

#include "GateGameLiftClient.h"
#include "GameLiftClientObject.h"

DEFINE_LOG_CATEGORY(GateGameLiftClientLog);

// Sets default values
AGateGameLiftClient::AGateGameLiftClient()
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient Construct!"));
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FString fleetid;
	if (FParse::Value(FCommandLine::Get(), TEXT("fleet"), fleetid, true)) {
		UE_LOG(GateGameLiftClientLog, Warning, TEXT("fleet=%s"), *fleetid);
		FleetId = fleetid;
	}
}

// Called when the game starts or when spawned
void AGateGameLiftClient::BeginPlay()
{
	Super::BeginPlay();

	// Add to Seamless Actor List on Local PC
	// TArray<AActor*> seamlsessActorList;
	// AActor::GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
}

// Called every frame
void AGateGameLiftClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGateGameLiftClient::Init(const FGameLiftClientData &clientData)
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient Init !"));
	if (gameLiftClientObj == nullptr)
	{
		gameLiftClientObj = UGameLiftClientObject::CreateGameLiftObject(
			clientData.awsCredentials,
			clientData.awsConfiguration);
	}
}

void AGateGameLiftClient::CreateGameSession(FCreateGameSessionRequest request)
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient CreateGameSession Invoked! [fleet:%s]"),*request.awsResourceName);
	FString gameSessionId;

	if (gameLiftClientObj != nullptr)
	{
		FLatentActionInfo latentInfo;
		latentInfo.CallbackTarget = this;
		latentInfo.ExecutionFunction = FName(TEXT("OnCreateGameSessionResult"));
		latentInfo.Linkage = 0;
		latentInfo.UUID = 0;

		gameLiftClientObj->CreateGameSession(this, isLastGameliftSuccess, request, createGameSessionResult, lastGameliftError, lastGameliftErrorMessage, latentInfo);
	}
}

void AGateGameLiftClient::CreatePlayerSession(FCreatePlayerSessionRequest request)
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient CreatePlayerSession Invoked! [gsession:%d]"),*request.awsResourceName);
	FString playerSessionId;

	if (gameLiftClientObj != nullptr)
	{
		FLatentActionInfo latentInfo;
		latentInfo.CallbackTarget = this;
		latentInfo.ExecutionFunction = FName(TEXT("OnCreatePlayerSessionResult"));
		latentInfo.Linkage = 0;
		latentInfo.UUID = 0;

		gameLiftClientObj->CreatePlayerSession(this, isLastGameliftSuccess, request, createPlayerSessionResult, lastGameliftError, lastGameliftErrorMessage, latentInfo);
	}
}

void AGateGameLiftClient::OnCreateGameSessionResult()
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient OnCreateGameSessionResult Invoked!"));
	if (!isLastGameliftSuccess)
	{
		UE_LOG(GateGameLiftClientLog, Warning, TEXT("Failed to create game session: %s[%d]"), *lastGameliftErrorMessage, (int32)lastGameliftError);
	}
	else
	{
		GameSessionId = createGameSessionResult.gameSession.gameSessionId;
	}
	OnGameSessionCreated.Broadcast(isLastGameliftSuccess, createGameSessionResult.gameSession.gameSessionId);
}

void AGateGameLiftClient::OnCreatePlayerSessionResult()
{
	UE_LOG(GateGameLiftClientLog, Warning, TEXT("Gate GameLiftClient OnCreatePlayerSessionResult Invoked!"));
	if (!isLastGameliftSuccess)
	{
		UE_LOG(GateGameLiftClientLog, Warning, TEXT("Failed to create player session: %s[%d]"), *lastGameliftErrorMessage, (int32)lastGameliftError);
	}
	else
		PlayerSessionId = createPlayerSessionResult.playerSession.playerSessionId;

	OnPlayerSessionCreated.Broadcast(
		isLastGameliftSuccess,
		createPlayerSessionResult.playerSession.playerSessionId,
		createPlayerSessionResult.playerSession.ipAddress,
		createPlayerSessionResult.playerSession.port);
}

void AGateGameLiftClient::SetisfyGameSession(const FString& sessionId)
{
	GameSessionId = sessionId;
}