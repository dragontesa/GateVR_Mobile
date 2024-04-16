// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ScriptDelegates.h"
#include "GatePixelStreamingActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GatePixelStreamingLog, Log, All);

UCLASS()
class GATEPIXELSTREAMING_API AGatePixelStreamingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGatePixelStreamingActor();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPixelStreamingFeatured, const FString&, FeatureId);
	UPROPERTY(BlueprintAssignable, Category = "Gate PixelStreaming")
	FOnPixelStreamingFeatured OnPixelStreamingFeatured;

    // UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gate PixelStreaming")

	class UPixelStreamingInput* GetPixelStreamingInputComp() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void OnPixelStreamingInputEvent(const FString& descripter);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	private:
	class UPixelStreamingInput* PixelStreamInputComp;
	TScriptDelegate<FWeakObjectPtr> HandlePixelStreamingInputEvent;

};
