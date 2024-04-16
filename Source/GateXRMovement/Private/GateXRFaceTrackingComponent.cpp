// Fill out your copyright notice in the Description page of Project Settings.
#include "GateXRFaceTrackingComponent.h"
#include "OculusXRHMD.h"
#include "OculusXRMovementFunctionLibrary.h"
#include "OculusXRPluginWrapper.h"
#include "OculusXRMovementHelpers.h"
#include "GateXRMovementLog.h"
#include "Kismet/KismetSystemLibrary.h"
#if ENABLE_REPLICATED_PROPERTY
#include "Net/UnrealNetwork.h"
#endif

int UGateXRFaceTrackingComponent::TrackingInstanceCount = 0;

// Sets default values for this component's properties
UGateXRFaceTrackingComponent::UGateXRFaceTrackingComponent()
   : TargetMeshComponentName(NAME_None)
	, InvalidFaceDataResetTime(2.0f)
	, bUpdateFace(true)
	, TargetMeshComponent(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Some defaults
	ExpressionNames.Add(EOculusXRFaceExpression::BrowLowererL, "browLowerer_L");
	ExpressionNames.Add(EOculusXRFaceExpression::BrowLowererR, "browLowerer_R");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekPuffL, "cheekPuff_L");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekPuffR, "cheekPuff_R");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekRaiserL, "cheekRaiser_L");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekRaiserR, "cheekRaiser_R");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekSuckL, "cheekSuck_L");
	ExpressionNames.Add(EOculusXRFaceExpression::CheekSuckR, "cheekSuck_R");
	ExpressionNames.Add(EOculusXRFaceExpression::ChinRaiserB, "chinRaiserB");
	ExpressionNames.Add(EOculusXRFaceExpression::ChinRaiserT, "chinRaiserT");
	ExpressionNames.Add(EOculusXRFaceExpression::DimplerL, "dimpler_L");
	ExpressionNames.Add(EOculusXRFaceExpression::DimplerR, "dimpler_R");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesClosedL, "eyesClosed_L");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesClosedR, "eyesClosed_R");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookDownL, "eyesLookDown_L");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookDownR, "eyesLookDown_R");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookLeftL, "eyesLookLeft_L");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookLeftR, "eyesLookLeft_R");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookRightL, "eyesLookRight_L");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookRightR, "eyesLookRight_R");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookUpL, "eyesLookUp_L");
	ExpressionNames.Add(EOculusXRFaceExpression::EyesLookUpR, "eyesLookUp_R");
	ExpressionNames.Add(EOculusXRFaceExpression::InnerBrowRaiserL, "innerBrowRaiser_L");
	ExpressionNames.Add(EOculusXRFaceExpression::InnerBrowRaiserR, "innerBrowRaiser_R");
	ExpressionNames.Add(EOculusXRFaceExpression::JawDrop, "jawDrop");
	ExpressionNames.Add(EOculusXRFaceExpression::JawSidewaysLeft, "jawSidewaysLeft");
	ExpressionNames.Add(EOculusXRFaceExpression::JawSidewaysRight, "jawSidewaysRight");
	ExpressionNames.Add(EOculusXRFaceExpression::JawThrust, "jawThrust");
	ExpressionNames.Add(EOculusXRFaceExpression::LidTightenerL, "lidTightener_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LidTightenerR, "lidTightener_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipCornerDepressorL, "lipCornerDepressor_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipCornerDepressorR, "lipCornerDepressor_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipCornerPullerL, "lipCornerPuller_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipCornerPullerR, "lipCornerPuller_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipFunnelerLB, "lipFunnelerLB");
	ExpressionNames.Add(EOculusXRFaceExpression::LipFunnelerLT, "lipFunnelerLT");
	ExpressionNames.Add(EOculusXRFaceExpression::LipFunnelerRB, "lipFunnelerRB");
	ExpressionNames.Add(EOculusXRFaceExpression::LipFunnelerRT, "lipFunnelerRT");
	ExpressionNames.Add(EOculusXRFaceExpression::LipPressorL, "lipPressor_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipPressorR, "lipPressor_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipPuckerL, "lipPucker_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipPuckerR, "lipPucker_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipStretcherL, "lipStretcher_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipStretcherR, "lipStretcher_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipSuckLB, "lipSuckLB");
	ExpressionNames.Add(EOculusXRFaceExpression::LipSuckLT, "lipSuckLT");
	ExpressionNames.Add(EOculusXRFaceExpression::LipSuckRB, "lipSuckRB");
	ExpressionNames.Add(EOculusXRFaceExpression::LipSuckRT, "lipSuckRT");
	ExpressionNames.Add(EOculusXRFaceExpression::LipTightenerL, "lipTightener_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LipTightenerR, "lipTightener_R");
	ExpressionNames.Add(EOculusXRFaceExpression::LipsToward, "lipsToward");
	ExpressionNames.Add(EOculusXRFaceExpression::LowerLipDepressorL, "lowerLipDepressor_L");
	ExpressionNames.Add(EOculusXRFaceExpression::LowerLipDepressorR, "lowerLipDepressor_R");
	ExpressionNames.Add(EOculusXRFaceExpression::MouthLeft, "mouthLeft");
	ExpressionNames.Add(EOculusXRFaceExpression::MouthRight, "mouthRight");
	ExpressionNames.Add(EOculusXRFaceExpression::NoseWrinklerL, "noseWrinkler_L");
	ExpressionNames.Add(EOculusXRFaceExpression::NoseWrinklerR, "noseWrinkler_R");
	ExpressionNames.Add(EOculusXRFaceExpression::OuterBrowRaiserL, "outerBrowRaiser_L");
	ExpressionNames.Add(EOculusXRFaceExpression::OuterBrowRaiserR, "outerBrowRaiser_R");
	ExpressionNames.Add(EOculusXRFaceExpression::UpperLidRaiserL, "upperLidRaiser_L");
	ExpressionNames.Add(EOculusXRFaceExpression::UpperLidRaiserR, "upperLidRaiser_R");
	ExpressionNames.Add(EOculusXRFaceExpression::UpperLipRaiserL, "upperLipRaiser_L");
	ExpressionNames.Add(EOculusXRFaceExpression::UpperLipRaiserR, "upperLipRaiser_R");
	// ...

	// Initialize Expression Values
	ExpressionDatas.Init(0.f,(int32)EOculusXRFaceExpression::COUNT);
}


// Called when the game starts
void UGateXRFaceTrackingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (UKismetSystemLibrary::IsDedicatedServer(this) || !UOculusXRMovementFunctionLibrary::IsFaceTrackingSupported())
	{
		// Early exit if face tracking isn't supported
		UE_LOG(LogGateXRMovement, Warning, TEXT("Face tracking is not supported. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}

	if (TargetMeshComponentName == NAME_None)
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Invalid mesh component name. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}

	if (!InitializeFaceTracking()) {
	    UE_LOG(LogGateXRMovement, Warning, TEXT("GateFt** Failed to initialize face tracking. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}
	
	if (!UOculusXRMovementFunctionLibrary::StartFaceTracking())
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Failed to start face tracking. (%s: %s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}
	
	++TrackingInstanceCount;
}

void UGateXRFaceTrackingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsComponentTickEnabled())
	{
		if (--TrackingInstanceCount == 0)
		{
			if (!UOculusXRMovementFunctionLibrary::StopFaceTracking())
			{
				UE_LOG(LogGateXRMovement, Warning, TEXT("GateFt** Failed to stop face tracking. (%s: %s)"), *GetOwner()->GetName(), *GetName());
			}
		}
	}

	UActorComponent::EndPlay(EndPlayReason);
}

// Called every frame
void UGateXRFaceTrackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(TargetMeshComponent))
	{
		UE_LOG(LogGateXRMovement, VeryVerbose, TEXT("No target mesh specified. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		return;
	}

	if (UOculusXRMovementFunctionLibrary::TryGetFaceState(FaceState) && bUpdateFace)
	{
		InvalidFaceStateTimer = 0.0f;

		for (int32 FaceExpressionIndex = 0; FaceExpressionIndex < static_cast<int32>(EOculusXRFaceExpression::COUNT); ++FaceExpressionIndex)
		{
			if (ExpressionValid[FaceExpressionIndex])
			{
				FName ExpressionName = ExpressionNames[static_cast<EOculusXRFaceExpression>(FaceExpressionIndex)];
				ExpressionDatas[FaceExpressionIndex] = FaceState.ExpressionWeights[FaceExpressionIndex];
				MorphTargets.SetMorphTarget(ExpressionName, FaceState.ExpressionWeights[FaceExpressionIndex]);
			}
		}
	}
	else
	{
	}

}

#if ENABLE_REPLICATED_PROPERTY
void UGateXRFaceTrackingComponent::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
  DOREPLIFETIME( UGateXRFaceTrackingComponent, ExpressionDatas );
}
#endif

void UGateXRFaceTrackingComponent::SetExpressionValue(EOculusXRFaceExpression Expression, float Value)
{
	if (Expression >= EOculusXRFaceExpression::COUNT)
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Cannot set expression value with invalid expression index."));
		return;
	}

	if (!ExpressionValid[static_cast<int32>(Expression)])
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Cannot set expression value for an expression with an invalid associated morph target name. Expression name: %s"), *StaticEnum<EOculusXRFaceExpression>()->GetValueAsString(Expression));
		return;
	}

	FName ExpressionName = ExpressionNames[Expression];
	MorphTargets.SetMorphTarget(ExpressionName, Value);
}

float UGateXRFaceTrackingComponent::GetExpressionValue(EOculusXRFaceExpression Expression) const
{
	if (Expression >= EOculusXRFaceExpression::COUNT)
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Cannot request expression value using an invalid expression index."));
		return 0.0f;
	}

	FName ExpressionName = ExpressionNames[Expression];
	if (ExpressionName == NAME_None)
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Cannot request expression value for an expression with an invalid associated morph target name. Expression name: %s"), *StaticEnum<EOculusXRFaceExpression>()->GetValueAsString(Expression));
		return 0.0f;
	}

    float exp = MorphTargets.GetMorphTarget(ExpressionName);
	// if (ExpressionName == "jawDrop")
    //    UE_LOG(LogGateXRMovement, Warning, TEXT("face tracked>> %s: %.2f"), *StaticEnum<EOculusXRFaceExpression>()->GetValueAsString(Expression), exp);
	   
	return exp;
	// return MorphTargets.GetMorphTarget(ExpressionName);
}

void UGateXRFaceTrackingComponent::ClearExpressionValues()
{
	MorphTargets.ClearMorphTargets();
}

bool UGateXRFaceTrackingComponent::InitializeFaceTracking()
{
	TargetMeshComponent = OculusXRUtility::FindComponentByName<USkinnedMeshComponent>(GetOwner(), TargetMeshComponentName);

	if (!IsValid(TargetMeshComponent))
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("GateFt** Could not find skeletal mesh component with name: (%s). (%s:%s)"), *TargetMeshComponentName.ToString(), *GetOwner()->GetName(), *GetName());
		return false;
	}

    if (TargetMeshComponent)
	{ // Assume all expression morp targets get it ready.
		for (const auto &it : ExpressionNames)
		{
			ExpressionValid[static_cast<int32>(it.Key)] = true;
		}
		return true;
	}

	return false;
}