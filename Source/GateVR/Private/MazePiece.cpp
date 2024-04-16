// Fill out your copyright notice in the Description page of Project Settings.


#include "MazePiece.h"

// Sets default values
AMazePiece::AMazePiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMazePiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMazePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

