// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MazePiece.h"
#include "MazeGenerator.generated.h"

UENUM(BlueprintType)
enum class EMazePieceType :uint8
{
    Straight UMETA(DisplayName="Straight"),
    Turn UMETA(DisplayName = "Turn"),
    TJunction UMETA(DisplayName = "TJunction"),
    DeadEnd UMETA(DisplayName = "DeadEnd"),
    Crossroad UMETA(DisplayName = "Crossroad"),
    Exit UMETA(DisplayName = "Exit")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GATEVR_API UMazeGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMazeGenerator();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    class MazeData {
    public:
        MazeData(UMazeGenerator& MazeGen) : MazeGen{ MazeGen } {}

        void Init() { Data.Init(1, MazeGen.Width * MazeGen.Height); }
        int8 Get(int32 X, int32 Y) const { return Data[X + Y * MazeGen.Width]; }
        void Set(int32 X, int32 Y, int8 value) { Data[X + Y * MazeGen.Width] = value; }
    private:
        UMazeGenerator& MazeGen;
        TArray<int8> Data;
        TArray<int8> PieceDirection;
        TArray<TSubclassOf<AMazePiece>> Piece;
    };
    MazeData Maze = MazeData(*this);

    class WorldDirections {
    public:
        void Shuffle() {
            for (int32 i = Data.Num() - 1; i != 0; i--) {
                int32 j = rand() % i;
                Data.Swap(i, j);
            }
        }
        int8& operator[](size_t i) { return Data[i]; }
        int32 Num() { return Data.Num(); }
    private:
        TArray<int8> Data = { 0, 1, 2, 3 };
    };
    WorldDirections Directions;

    void Step(int32 X, int32 Y);
    void TJunctionToCross(int32 Count);
    void UpdatePieceData();
    bool IsPatternMatching(int32 X, int32 Y, TArray<int8> Pattern) const;

    TArray<UMazePieceData*> PieceData;
    TArray<UMazePieceData*> GetPiecesByType(EMazePieceType type);

    // Straight
    TArray<int8> HorizontalStraightPattern = { 5, 1, 5,
                                               0, 0, 0,
                                               5, 1, 5 };

    TArray<int8> VerticalStraightPattern = { 5, 0, 5,
                                             1, 0, 1,
                                             5, 0, 5 };

    // T Junctions	
    TArray<int8> TJunctionUpPattern = { 1, 0, 1,
                                        0, 0, 0,
                                        5, 1, 5 };

    TArray<int8> TJunctionDownPattern = { 5, 1, 5,
                                          0, 0, 0,
                                          1, 0, 1 };

    TArray<int8> TJunctionLeftPattern = { 1, 0, 5,
                                          0, 0, 1,
                                          1, 0, 5 };

    TArray<int8> TJunctionRightPattern = { 5, 0, 1,
                                           1, 0, 0,
                                           5, 0, 1 };

    // Crossroad
    TArray<int8> CrossroadPattern = { 1, 0, 1,
                                      0, 0, 0,
                                      1, 0, 1 };

    // Turns
    TArray<int8> TurnLeftUpPattern = { 1, 0, 5,
                                       0, 0, 1,
                                       5, 1, 5 };

    TArray<int8> TurnLeftDownPattern = { 5, 1, 5,
                                         0, 0, 1,
                                         1, 0, 5 };

    TArray<int8> TurnRightUpPattern = { 5, 0, 1,
                                        1, 0, 0,
                                        5, 1, 5 };

    TArray<int8> TurnRightDownPattern = { 5, 1, 5,
                                          1, 0, 0,
                                          5, 0, 1 };

    // Dead ends
    TArray<int8> DeadEndUpPattern = { 5, 0, 5,
                                      1, 0, 1,
                                      5, 1, 5 };

    TArray<int8> DeadEndDownPattern = { 5, 1, 5,
                                        1, 0, 1,
                                        5, 0, 5 };

    TArray<int8> DeadEndLeftPattern = { 5, 1, 5,
                                        0, 0, 1,
                                        5, 1, 5 };

    TArray<int8> DeadEndRightPattern = { 5, 1, 5,
                                         1, 0, 0,
                                         5, 1, 5 };

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
        TArray<UMazePieceData*> Generate();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 StartX = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 StartY = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Width = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Height = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Scale = 600;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 BridgeCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 ExitCount = 1;
};

UCLASS(BlueprintType)
class UMazePieceData : public UObject
{
    GENERATED_BODY()

public:

    UMazePieceData() {
        x = 0;
        y = 0;
        area = 1;
        angle = 0;
        type = EMazePieceType::Straight;
    }

    void Init(int32 X, int32 Y, int32 Area, int32 Angle, EMazePieceType Type) {
        x = X; y = Y; area = Area; angle = Angle; type = Type;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 x;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 y;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 area;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 angle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        EMazePieceType type;
};
