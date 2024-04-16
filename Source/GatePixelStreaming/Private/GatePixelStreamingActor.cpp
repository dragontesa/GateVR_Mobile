// Fill out your copyright notice in the Description page of Project Settings.
#include "GatePixelStreamingActor.h"
#include "PixelStreamingInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Misc/CoreMiscDefines.h"

DEFINE_LOG_CATEGORY(GatePixelStreamingLog);

// Sets default values
AGatePixelStreamingActor::AGatePixelStreamingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HandlePixelStreamingInputEvent.BindUFunction(this, TEXT("OnPixelStreamingInputEvent"));
}

// Called when the game starts or when spawned
void AGatePixelStreamingActor::BeginPlay()
{
	Super::BeginPlay();
	UKismetSystemLibrary::ExecuteConsoleCommand(this, TEXT("EnableAllScreenMessages"));
	UKismetSystemLibrary::ExecuteConsoleCommand(this, TEXT("r.Streaming.PoolSize 8000"));
	APlayerCameraManager * cm = UGameplayStatics::GetPlayerCameraManager(this, 0);
	cm->SetManualCameraFade(1.0f,FLinearColor(EForceInit::ForceInit),false);
	FLatentActionInfo laten1;
	UGameplayStatics::LoadStreamLevel(this, TEXT("FreezeFrame"),false, true, laten1 );
	FLatentActionInfo laten2;
	UGameplayStatics::LoadStreamLevel(this, TEXT("AudioComponent"),false, true, laten2 );
	FLatentActionInfo laten3;
	UGameplayStatics::LoadStreamLevel(this, TEXT("PSStats"),false, true, laten3 );
	FLatentActionInfo laten4;
	UKismetSystemLibrary::Delay(this, 1.0f, laten4);

	cm->StartCameraFade(1.0f,0.0f,2.0f,FLinearColor(EForceInit::ForceInit),false,true);

	// APlayerController* pc = UGameplayStatics::GetPlayerController(this,0);
	// ACharacter* c = UGameplayStatics::GetPlayerCharacter(this,0);
	// UPrimitiveComponent* sic = NewObject<UPixelStreamingInput>(c);
	UActorComponent* ac = AActor::AddComponentByClass(UPixelStreamingInput::StaticClass(),false,FTransform(),false);
	PixelStreamInputComp = static_cast<UPixelStreamingInput*>(ac);
	PixelStreamInputComp->OnInputEvent.Add(HandlePixelStreamingInputEvent);
}

// Called every frame
void AGatePixelStreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGatePixelStreamingActor::OnPixelStreamingInputEvent(const FString& descripter)
{
	UE_LOG(GatePixelStreamingLog, Warning, TEXT("Got OnPixelStreamInputEvent!"));
	if (!PixelStreamInputComp)
	    return;

    FString k4(TEXT("4k"));
    if (UKismetStringLibrary::Contains(descripter,k4))
	{
		FString cl = UKismetSystemLibrary::GetCommandLine();
		FString nvencH264(TEXT("-NvEncH264ConfigLevel=NV_ENC_LEVEL_H264_52"));
		if (!UKismetStringLibrary::Contains(cl,nvencH264))
		{
			PixelStreamInputComp->SendPixelStreamingResponse(TEXT(""));
			UE_LOG(GatePixelStreamingLog, Warning, TEXT("Sent PixelStreaming Response for NvencH264"));
		}
	}
}

UPixelStreamingInput* AGatePixelStreamingActor::GetPixelStreamingInputComp() const
{
	return PixelStreamInputComp;
}