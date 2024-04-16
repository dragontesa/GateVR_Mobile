// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazePiece.generated.h"

UCLASS()
class GATEVR_API AMazePiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazePiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 x;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 area;

};
