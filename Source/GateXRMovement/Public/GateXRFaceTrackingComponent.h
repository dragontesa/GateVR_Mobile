// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GateXRMorphTargetsController.h"
#include "OculusXRMovementTypes.h"
#include "GateXRFaceTrackingComponent.generated.h"
#define ENABLE_REPLICATED_PROPERTY 0

// UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent, DisplayName = "GateXR Face Tracking By OculusXR Movement "), ClassGroup = OculusXRHMD)
class GATEXRMOVEMENT_API UGateXRFaceTrackingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGateXRFaceTrackingComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Property Replication Register
	#if ENABLE_REPLICATED_PROPERTY
    void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	#endif
	/**
	 * Set face expression value with expression key and value(0-1).
	 *
	 * @param Expression : The expression key that will be modified.
	 * @param Value : The new value to assign to the expression, 0 will remove all changes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|OculusXRFaceTracking", meta = (UnsafeDuringActorConstruction = "true"))
	void SetExpressionValue(EOculusXRFaceExpression Expression, float Value);

	/**
	 * Get a face expression value given an expression key.
	 *
	 * @param Expression : The expression key that will be queried.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|OculusXRFaceTracking")
	float GetExpressionValue(EOculusXRFaceExpression Expression) const;

	/**
	 * Clears all face expression values.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|OculusXRFaceTracking")
	void ClearExpressionValues();

	/**
	 * The name of the skinned mesh component that this component targets for facial expression.
	 * This must be the name of a component on this actor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OculusXR|Movement")
	FName TargetMeshComponentName;

	/**
	 * If the face data is invalid for at least this or longer than this time then all face blendshapes/morph targets are reset to zero.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OculusXR|Movement")
	float InvalidFaceDataResetTime;

	/**
	 * The list of expressions that this component supports.
	 * Names are validated on startup so only valid morph targets on the skeletal mesh will be targeted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OculusXR|Movement")
	TMap<EOculusXRFaceExpression, FName> ExpressionNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OculusXR|Movement")
	TArray<float> ExpressionDatas;

	/**
	* This flag determines if the face should be updated or not during the components tick.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oculus|Movement")
	bool bUpdateFace;

private:
	bool InitializeFaceTracking();

	// The mesh component targeted for expressions
	UPROPERTY()
	USkinnedMeshComponent* TargetMeshComponent;

	// Which mapped expressions are valid
	TStaticArray<bool, static_cast<uint32>(EOculusXRFaceExpression::COUNT)> ExpressionValid;

	// Morph targets controller
	FGateXRMorphTargetsController MorphTargets;

	FOculusXRFaceState FaceState;

	// Timer that counts up until we reset morph curves if we've failed to get face state
	float InvalidFaceStateTimer;

	// Stop the tracker just once.
	static int TrackingInstanceCount;
		
};
