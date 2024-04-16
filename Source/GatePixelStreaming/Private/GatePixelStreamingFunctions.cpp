#include "GatePixelStreamingFunctions.h"
#include "Kismet/GameplayStatics.h"
// #include "Components/ActorComponent.h"
#include "PixelStreamingInputComponent.h"

AGatePixelStreamingActor* UGatePixelStreamingFunctions::streamerActor = nullptr;
UPixelStreamingInput* UGatePixelStreamingFunctions::streamInputComp = nullptr;

void UGatePixelStreamingFunctions::SetupPixelStreaming(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		streamerActor = World->SpawnActor<AGatePixelStreamingActor>(AGatePixelStreamingActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	}
}

void UGatePixelStreamingFunctions::ReleasePixelStreaming()
{
	if (streamerActor != nullptr) {
		if(APlayerController* player = UGameplayStatics::GetPlayerController(streamerActor,0))
		{
			UPixelStreamingInput* pi = player->GetComponentByClass<UPixelStreamingInput>();
			if (pi)
			{
				// player->RemoveOwnedComponent(pi);
				pi->DestroyComponent();
			}
		}
		streamerActor->Destroy();
		streamerActor = nullptr;
	}
}

void UGatePixelStreamingFunctions::SwitchViewTarget(AActor * NewViewTarget)
{
	if (streamerActor == nullptr)
	{
		UE_LOG(GatePixelStreamingLog, Warning, TEXT("Streamer is not ready, Try to initialize!"));
		return;
	}

	if (!NewViewTarget)
	{
        UE_LOG(GatePixelStreamingLog, Warning, TEXT("Invalid View Target!"));
		return;
	}

	APlayerController* player = UGameplayStatics::GetPlayerController(streamerActor,0);
	if (player != nullptr) {
		player->SetViewTargetWithBlend(NewViewTarget);
	}
	else {
		UE_LOG(GatePixelStreamingLog, Warning, TEXT("Failed to change view target >> Invalid Player!"));
	}
}


AGatePixelStreamingActor* UGatePixelStreamingFunctions::GetPixelStreamer()
{
	return streamerActor;
}


UPixelStreamingInput* UGatePixelStreamingFunctions::GetPixelStreamerInput()
{
    return streamerActor->GetPixelStreamingInputComp();
}