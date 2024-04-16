// Fill out your copyright notice in the Description page of Project Settings.


#include "GateGameLiftGameMode.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#endif

DEFINE_LOG_CATEGORY(GateGameLiftServerLog);

#if WITH_GAMELIFT
TSharedRef<FProcessParameters> params = MakeShared<FProcessParameters>();
#endif

bool AGateGameLiftGameMode::IsGameLiftServerInited = false;

AGateGameLiftGameMode::AGateGameLiftGameMode()
:  IsAnywhere(false),
   TurnOffGameLift(false),
   ListenPort(7777)
{
    UE_LOG(GateGameLiftServerLog, Log, TEXT("Gatevr GameLift Game Mode Construct"));
    #if WITH_GAMELIFT
    // // set default pawn class to our Blueprinted character
    // static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Multiplayer/GameLift/Pawn_GameLift"));
    // if (PlayerPawnBPClass.Class != NULL)
    // {
    //     DefaultPawnClass = PlayerPawnBPClass.Class;
    // }
    if (FParse::Param(FCommandLine::Get(), TEXT("DisableGamelift")))
    {
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Gamelift get disabled by commandline"));
        TurnOffGameLift = true;
    }

    if (FParse::Param(FCommandLine::Get(), TEXT("Anywhere")))
    {
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Started as Anywhere"));
        IsAnywhere = true;
    }
    else
    {
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Started as EC2"));
        IsAnywhere = false;
    }

    if (FParse::Value(FCommandLine::Get(), TEXT("port="), ListenPort)) {
		UE_LOG(GateGameLiftServerLog, Warning, TEXT("listenPort=%u"), ListenPort);
    }

	if (FParse::Value(FCommandLine::Get(), TEXT("fleet"), FleetId, true)) {
		UE_LOG(GateGameLiftServerLog, Warning, TEXT("fleet=%s"), *FleetId);
	}
    if (FParse::Value(FCommandLine::Get(), TEXT("hostname"), HostName, true)) {
		UE_LOG(GateGameLiftServerLog, Warning, TEXT("host=%s"), *HostName);
	}
    if (FParse::Value(FCommandLine::Get(), TEXT("auth"), AuthToken, true)) {
		UE_LOG(GateGameLiftServerLog, Warning, TEXT("auth=%s"), *AuthToken);
	}
    if (FParse::Value(FCommandLine::Get(), TEXT("web"), WebSocketUrl, true)) {
		UE_LOG(GateGameLiftServerLog, Warning, TEXT("web=%s"), *WebSocketUrl);
	}
    #endif
}

void AGateGameLiftGameMode::BeginPlay()
{
#if WITH_GAMELIFT
    if (TurnOffGameLift == false)
    {
        if (!AGateGameLiftGameMode::IsGameLiftServerInited)
            InitGameLiftSDK();
    }
    else 
    {
       UE_LOG(GateGameLiftServerLog, Log, TEXT("Normal Server Begin ..."));
       IsGameLiftServerInited = false;
    }
#endif
}


void AGateGameLiftGameMode::InitGameLiftSDK()
{
    UE_LOG(GateGameLiftServerLog, Log, TEXT("Initializing the GameLift Server"));
#if WITH_GAMELIFT
    // Getting the module first.
    FGameLiftServerSDKModule *gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    // check version
    FGameLiftStringOutcome ver = gameLiftSdkModule->GetSdkVersion();
    UE_LOG(GateGameLiftServerLog, Log, TEXT("GameLift SDK ver ----------  %s"), *ver.GetResult());
    
    // SDK >= 5
    #if GAMELIFT_SERVER_SDK_VER > 4
    // Define the server parameters
    FServerParameters serverParameters;

    // This is for Anywhere
    if (IsAnywhere)
    {
        serverParameters.m_authToken = AuthToken;
        serverParameters.m_hostId = HostName;
        serverParameters.m_fleetId = FleetId;
        serverParameters.m_webSocketUrl = WebSocketUrl;

        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Initializing GameServer As Anywhere !!"));
        // AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
        if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), serverParameters.m_authToken))
        {
            UE_LOG(GateGameLiftServerLog, Log, TEXT("AUTH_TOKEN: %s"), *serverParameters.m_authToken)
        }

        // The Host/Compute ID of the GameLift Anywhere instance.
        if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), serverParameters.m_hostId))
        {
            UE_LOG(GateGameLiftServerLog, Log, TEXT("HOST_ID: %s"), *serverParameters.m_hostId)
        }

        // The EC2 or Anywhere Fleet ID.
        if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), serverParameters.m_fleetId))
        {
            UE_LOG(GateGameLiftServerLog, Log, TEXT("FLEET_ID: %s"), *serverParameters.m_fleetId)
        }

        // The WebSocket URL (GameLiftServiceSdkEndpoint).
        if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), serverParameters.m_webSocketUrl))
        {
            UE_LOG(GateGameLiftServerLog, Log, TEXT("WEBSOCKET_URL: %s"), *serverParameters.m_webSocketUrl)
        }

        // The PID of the running process
        serverParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
        UE_LOG(GateGameLiftServerLog, Log, TEXT("PID: %s"), *serverParameters.m_processId);
    }
    else
    { // this is for EC2
        UE_LOG(GateGameLiftServerLog, Warning, TEXT("Initializing GameServer As Managed EC2 !!"));
        serverParameters.m_webSocketUrl = nullptr;
        serverParameters.m_fleetId = nullptr;
        serverParameters.m_processId = nullptr;
        serverParameters.m_hostId = nullptr;
        serverParameters.m_authToken = nullptr;
    }

    // InitSDK will establish a local connection with GameLift's agent to enable further communication.
        auto initResult = gameLiftSdkModule->InitSDK(serverParameters);
    #else // SDK < 5.0 
        auto initResult = gameLiftSdkModule->InitSDK();
    #endif

    if (initResult.IsSuccess())
    {
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Init GameLift SDK Okay!"));
    }
    else
    {
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Init GameLift SDK Failed!: %s"),*(initResult.GetError().m_errorMessage));
    }

    // When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
    // Here is where a game server should take action based on the game session object.
    // Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
    auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
    {
        FString gameSessionId = FString(gameSession.GetGameSessionId());
        UE_LOG(GateGameLiftServerLog, Log, TEXT("GameSession Initializing: %s"), *gameSessionId);
        gameLiftSdkModule->ActivateGameSession();
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Activated GameSession"));
    };

    params->OnStartGameSession.BindLambda(onGameSession);

    // OnProcessTerminate callback. GameLift will invoke this callback before shutting down an instance hosting this game server.
    // It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
    // In this case, we simply tell GameLift we are indeed going to shutdown.
    params->OnTerminate.BindLambda([=]() {
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Game Server Process is terminating"));
        gameLiftSdkModule->ProcessEnding(); });

    // This is the HealthCheck callback.
    // GameLift will invoke this callback every 60 seconds or so.
    // Here, a game server might want to check the health of dependencies and such.
    // Simply return true if healthy, false otherwise.
    // The game server has 60 seconds to respond with its health status. GameLift will default to 'false' if the game server doesn't respond in time.
    // In this case, we're always healthy!
    params->OnHealthCheck.BindLambda([]() {
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Performing Health Check"));
        return true; });

    // This game server tells GameLift that it will listen on port 7777 for incoming player connections.
    params->port = ListenPort;

    // Here, the game server tells GameLift what set of files to upload when the game session ends.
    // GameLift will upload everything specified here for the developers to fetch later.
    TArray<FString> logfiles;
    logfiles.Add(TEXT("C:/Game/GateVR/Saved/Logs/GateVR.log"));
    params->logParameters = logfiles;

    // Calling ProcessReady tells GameLift this game server is ready to receive incoming game sessions!
    UE_LOG(GateGameLiftServerLog, Log, TEXT("Calling Process Ready"));
    auto result = gameLiftSdkModule->ProcessReady(*params);
    if (result.IsSuccess())
    {
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Ready Okay!!"));
    }
    else
    {
        FGameLiftError glError = result.GetError();
        UE_LOG(GateGameLiftServerLog, Log, TEXT("Ready Failed: %s"), *(glError.m_errorMessage));
    }

    UE_LOG(GateGameLiftServerLog, Log, TEXT("Initializing the GameLift Server End!"));
    AGateGameLiftGameMode::IsGameLiftServerInited = true;
#endif
}


void AGateGameLiftGameMode::PreLogin(const FString &Options, const FString &Address, const FUniqueNetIdRepl &UniqueId, FString &ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    UE_LOG(GateGameLiftServerLog, Warning, TEXT("prelogin started!"));
#if WITH_GAMELIFT
    UE_LOG(GateGameLiftServerLog, Warning, TEXT("prelogin serverside !"));
    UE_LOG(GateGameLiftServerLog, Warning, TEXT("login options: %s"), *Options);
    if (TurnOffGameLift == false)
    {
        FString PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
        Aws::GameLift::GenericOutcome Outcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
        if (Outcome.IsSuccess())
        {
            UE_LOG(GateGameLiftServerLog, Warning, TEXT("Accept Success!: %d"), Outcome.GetResult());
        }
        else
        {
            UE_LOG(GateGameLiftServerLog, Warning, TEXT("Accept Failed! : [%d] %s"), (int)Outcome.GetError().GetErrorType(), Outcome.GetError().GetErrorMessage());
            ErrorMessage = "AcceptPlayerSessionFailed. Code=" + FString::FromInt((int)Outcome.GetError().GetErrorType());
        }
    }
#endif
}
