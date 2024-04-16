// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeGenerator.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMazeGenerator::UMazeGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMazeGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<UMazePieceData*> UMazeGenerator::Generate() {
    TArray<UMazePieceData*> Temp;
	Maze.Init();
    PieceData.Empty();
    Step(StartX, StartY);
    TJunctionToCross(BridgeCount);
    UpdatePieceData();
    for (int i = 0; i < PieceData.Num(); i++) {
        Temp.Add(PieceData[i]);
    }
    return Temp;
}

void UMazeGenerator::Step(int32 X, int32 Y) // Generate Data By Repeat Step Until Done
{
    Directions.Shuffle();

    for (int32 i = 0; i < Directions.Num(); i++) {
        switch (Directions[i])
        {
        case 0: // Up
            if (X + 2 >= Width - 1 || Maze.Get(X + 2, Y) == 0)
                continue;

            Maze.Set(X + 2, Y, 0);
            Maze.Set(X + 1, Y, 0);
            Step(X + 2, Y);

        case 1: // Right
            if (Y + 2 >= Height - 1 || Maze.Get(X, Y + 2) == 0)
                continue;

            Maze.Set(X, Y + 2, 0);
            Maze.Set(X, Y + 1, 0);
            Step(X, Y + 2);

        case 2: // Down
            if (X - 2 <= 0 || Maze.Get(X - 2, Y) == 0)
                continue;

            Maze.Set(X - 2, Y, 0);
            Maze.Set(X - 1, Y, 0);
            Step(X - 2, Y);

        case 3: // Left
            if (Y - 2 <= 0 || Maze.Get(X, Y - 2) == 0)
                continue;

            Maze.Set(X, Y - 2, 0);
            Maze.Set(X, Y - 1, 0);
            Step(X, Y - 2);
        }
    }
}

void UMazeGenerator::TJunctionToCross(int32 Count) { // Add Data for make CrossPiece from TJunctionPiece

    TArray<FVector2D*> TJunctionPositions;
    for (int32 x = 2; x < Width - 2; x++)
    {
        for (int32 y = 2; y < Height - 2; y++)
        {
            if (IsPatternMatching(x, y, TJunctionUpPattern)) { TJunctionPositions.Add(new FVector2D(x, y)); }
            else if (IsPatternMatching(x, y, TJunctionDownPattern)) { TJunctionPositions.Add(new FVector2D(x, y)); }
            else if (IsPatternMatching(x, y, TJunctionLeftPattern)) { TJunctionPositions.Add(new FVector2D(x, y)); }
            else if (IsPatternMatching(x, y, TJunctionRightPattern)) { TJunctionPositions.Add(new FVector2D(x, y)); }
        }
    }

    if (Count >= TJunctionPositions.Num() - 1) Count = TJunctionPositions.Num() - 1;

    for (int i = 0; i < Count; i++) {
        int index = FMath::RandRange(0, TJunctionPositions.Num() - 1);
        FVector2D pos = *TJunctionPositions[index];

        if (Maze.Get(pos.X - 1, pos.Y) == 1) { Maze.Set(pos.X - 1, pos.Y, 0); }
        else if (Maze.Get(pos.X + 1, pos.Y) == 1) { Maze.Set(pos.X + 1, pos.Y, 0); }
        else if (Maze.Get(pos.X, pos.Y - 1) == 1) { Maze.Set(pos.X, pos.Y - 1, 0); }
        else if (Maze.Get(pos.X, pos.Y + 1) == 1) { Maze.Set(pos.X, pos.Y + 1, 0); }

        TJunctionPositions.RemoveAt(index);
    }
}

void UMazeGenerator::UpdatePieceData()
{
    int32 HalfWidth = Width / 2;
    int32 HalfHeight = Height / 2;
    int32 area = 1;

    for (int32 x = 1; x < Width - 1; x++)
    {
        for (int32 y = 1; y < Height - 1; y++)
        {
            
            if (x > HalfWidth && y < HalfHeight) area = 1;
            else if (x > HalfWidth && y > HalfHeight) area = 2;
            else if (x < HalfWidth && y < HalfHeight) area = 3;
            else if (x < HalfWidth && y > HalfHeight) area = 4;

            UMazePieceData* Data = NewObject<UMazePieceData>(this);

            // Straight
            if (IsPatternMatching(x, y, HorizontalStraightPattern)) { Data->Init(x, y, area, 90.f, EMazePieceType::Straight); }
            else if (IsPatternMatching(x, y, VerticalStraightPattern)) { Data->Init(x, y, area, 0.f, EMazePieceType::Straight); }

            // Turns
            else if (IsPatternMatching(x, y, TurnLeftUpPattern)) { Data->Init(x, y, area, 0.f, EMazePieceType::Turn); }
            else if (IsPatternMatching(x, y, TurnLeftDownPattern)) { Data->Init(x, y, area, 90.f, EMazePieceType::Turn); }
            else if (IsPatternMatching(x, y, TurnRightUpPattern)) { Data->Init(x, y, area, -90.f, EMazePieceType::Turn); }
            else if (IsPatternMatching(x, y, TurnRightDownPattern)) { Data->Init(x, y, area, 180.f, EMazePieceType::Turn); }

            // T Junctions	
            else if (IsPatternMatching(x, y, TJunctionUpPattern)) { Data->Init(x, y, area, -90.f, EMazePieceType::TJunction); }
            else if (IsPatternMatching(x, y, TJunctionDownPattern)) { Data->Init(x, y, area, 90.f, EMazePieceType::TJunction); }
            else if (IsPatternMatching(x, y, TJunctionLeftPattern)) { Data->Init(x, y, area, 0.f, EMazePieceType::TJunction); }
            else if (IsPatternMatching(x, y, TJunctionRightPattern)) { Data->Init(x, y, area, 180.f, EMazePieceType::TJunction); }

            // Dead ends
            else if (IsPatternMatching(x, y, DeadEndUpPattern)) { Data->Init(x, y, area, 90.f, EMazePieceType::DeadEnd); }
            else if (IsPatternMatching(x, y, DeadEndDownPattern)) { Data->Init(x, y, area, -90.f, EMazePieceType::DeadEnd); }
            else if (IsPatternMatching(x, y, DeadEndLeftPattern)) { Data->Init(x, y, area, 180.f, EMazePieceType::DeadEnd); }
            else if (IsPatternMatching(x, y, DeadEndRightPattern)) { Data->Init(x, y, area, 0.f, EMazePieceType::DeadEnd); }

            // Crossroad
            else if (IsPatternMatching(x, y, CrossroadPattern)) { Data->Init(x, y, area, 0.f, EMazePieceType::Crossroad); }

            if(Data->x != 0) PieceData.Add(Data);
        }
    }

    TArray<UMazePieceData*> DeadEnds = GetPiecesByType(EMazePieceType::DeadEnd);
    if (ExitCount > DeadEnds.Num()) ExitCount = DeadEnds.Num();
    for (int i = 0; i < ExitCount; i++) {
        int32 index = FMath::RandRange(0, DeadEnds.Num()-1);
        DeadEnds[index]->type = EMazePieceType::Exit;
        DeadEnds.RemoveAt(index);
    }
}

TArray<UMazePieceData*> UMazeGenerator::GetPiecesByType(EMazePieceType type) {
    TArray<UMazePieceData*> Result;
    for (int i = 0; i < PieceData.Num(); i++) {
        if (PieceData[i]->type == type) Result.Add(PieceData[i]);
    }
    return Result;
}

bool UMazeGenerator::IsPatternMatching(int32 X, int32 Y, TArray<int8> Pattern) const
{
    int Count = 0;
    int i = 0;
    for (int y = 1; y > -2; y--)
    {
        for (int x = -1; x < 2; x++)
        {
            if (Pattern[i] == Maze.Get(X + x, Y + y) || Pattern[i] == 5)
            {
                Count++;
            }

            i++;
        }
    }

    return Count == 9;
}