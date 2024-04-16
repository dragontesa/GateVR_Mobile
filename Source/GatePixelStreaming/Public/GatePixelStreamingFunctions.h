// #ifndef B8D2F693_4C84_496C_9B9E_85C401CC1252
// #define B8D2F693_4C84_496C_9B9E_85C401CC1252
#pragma once

#include "CoreMinimal.h"
#include "GatePixelStreamingActor.h"
#include "PixelStreamingInputComponent.h"
#include "Components/ActorComponent.h"
#include "GatePixelStreamingFunctions.generated.h"

UCLASS()
class GATEPIXELSTREAMING_API UGatePixelStreamingFunctions : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    public:

    UFUNCTION(BlueprintCallable, meta=(DisplayName = "Setup Gate PixelStreaming"), Category="GatePixelStreaming")
    static void SetupPixelStreaming(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, meta=(DisplayName = "Release Gate PixelStreaming"), Category="GatePixelStreaming")
    static void ReleasePixelStreaming();

    UFUNCTION(BlueprintCallable, meta=(DisplayName = "Switch Gate Pixelstreaming View Target"), Category="GatePixelStreaming")
    static void SwitchViewTarget(AActor * NewViewTarget);

    UFUNCTION(BlueprintCallable, meta=(DisplayName = "Get Gate PixelStreamer"), Category="GatePixelStreaming")
    static AGatePixelStreamingActor* GetPixelStreamer();

    UFUNCTION(BlueprintCallable, meta=(DisplayName = "Get Gate PixelStreamer Input"), Category="GatePixelStreaming")
    static UPixelStreamingInput* GetPixelStreamerInput();

    private:
    static AGatePixelStreamingActor* streamerActor; // This actor should be singleton per a Player and should not be replicated
    static UPixelStreamingInput* streamInputComp;

};

// #endif /* B8D2F693_4C84_496C_9B9E_85C401CC1252 */
