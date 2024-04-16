#include "GateXREyeTrackingComponent.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/PlayerController.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRPluginWrapper.h"
#include "OculusXRMovementFunctionLibrary.h"
#include "OculusXRMovementHelpers.h"
#include "GateXRMovementLog.h"

int UGateXREyeTrackingComponent::TrackingInstanceCount = 0;

FGateXREyeGazeState::FGateXREyeGazeState()
	: EyeRotation(FQuat(0.0f,0.0f,0.0f,0.0f))
	, EyeLocation(FVector::ZeroVector)
	, Confidence(0.f)
	, bIsValid(false)
{
}

UGateXREyeTrackingComponent::UGateXREyeTrackingComponent()
	: TargetMeshComponentName(NAME_None)
	, bUpdatePosition(true)
	, bUpdateRotation(true)
	, ConfidenceThreshold(0.f)
	, bAcceptInvalid(false)
	, WorldToMeters(100.f)
	, TargetPoseableMeshComponent(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	EyeToBone.Add(EOculusXREye::Left, "LeftEye");
	EyeToBone.Add(EOculusXREye::Right, "RightEye");

	EyeGazeStereoStates.Init(FGateXREyeGazeState(),(int32)EOculusXREye::COUNT);
}

void UGateXREyeTrackingComponent::BeginPlay()
{
	Super::BeginPlay();
 #if 1 // UE5.3.2 + MetaXR 60, this get failed
	if (!UOculusXRMovementFunctionLibrary::IsEyeTrackingSupported())
	{
		// Early exit if eye tracking isn't supported
		UE_LOG(LogGateXRMovement, Warning, TEXT("Eye tracking is not supported. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}
#endif
	// Try & check initializing the eye data
	if (!InitializeEyes())
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Failed to initialize eye tracking data. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
	}

	if (!UOculusXRMovementFunctionLibrary::StartEyeTracking())
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Failed to start eye tracking. (%s: %s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}
	++TrackingInstanceCount;
}

void UGateXREyeTrackingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsComponentTickEnabled())
	{
		if (--TrackingInstanceCount == 0)
		{
			if (!UOculusXRMovementFunctionLibrary::StopEyeTracking())
			{
				UE_LOG(LogGateXRMovement, Warning, TEXT("Failed to stop eye tracking. (%s: %s)"), *GetOwner()->GetName(), *GetName());
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UGateXREyeTrackingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if WITH_EYETRACKING_POSEABLEMESH
	if (!IsValid(TargetPoseableMeshComponent))
	{
		UE_LOG(LogGateXRMovement, VeryVerbose, TEXT("No target mesh specified. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		SetComponentTickEnabled(false);
		return;
	}
#endif
	FOculusXREyeGazesState EyeGazesState;

	if (UOculusXRMovementFunctionLibrary::TryGetEyeGazesState(EyeGazesState, WorldToMeters))
	{
		for (uint8 i = 0u; i < static_cast<uint8>(EOculusXREye::COUNT); ++i)
		{
			if (PerEyeData[i].EyeIsMapped)
			{
				const auto& Bone = PerEyeData[i].MappedBoneName;
				const auto& EyeGaze = EyeGazesState.EyeGazes[i];
				// GateXR Stuff
				EyeGazeStereoStates[i].EyeRotation = EyeGaze.Orientation.Quaternion() * PerEyeData[i].InitialRotation;
				EyeGazeStereoStates[i].EyeLocation = EyeGaze.Position;
				EyeGazeStereoStates[i].Confidence = EyeGaze.Confidence;
				EyeGazeStereoStates[i].bIsValid = EyeGaze.bIsValid;

				#if WITH_EYETRACKING_POSEABLEMESH
				if ((bAcceptInvalid || EyeGaze.bIsValid) && (EyeGaze.Confidence >= ConfidenceThreshold))
				{
					int32 BoneIndex = TargetPoseableMeshComponent->GetBoneIndex(Bone);
					FTransform CurrentTransform = TargetPoseableMeshComponent->GetBoneTransformByName(Bone, EBoneSpaces::ComponentSpace);

					if (bUpdatePosition)
					{
						CurrentTransform.SetLocation(EyeGaze.Position);
					}

					if (bUpdateRotation)
					{
						CurrentTransform.SetRotation(EyeGaze.Orientation.Quaternion() * PerEyeData[i].InitialRotation);
					}

					TargetPoseableMeshComponent->SetBoneTransformByName(Bone, CurrentTransform, EBoneSpaces::ComponentSpace);
				}
				#endif
			}
		}
	}
	else
	{
		UE_LOG(LogGateXRMovement, VeryVerbose, TEXT("Failed to get Eye state from EyeTrackingComponent. (%s:%s)"), *GetOwner()->GetName(), *GetName());
	}
}

void UGateXREyeTrackingComponent::ClearRotationValues()
{
	#if WITH_EYETRACKING_POSEABLEMESH
	if (!IsValid(TargetPoseableMeshComponent))
	{
		UE_LOG(LogGateXRMovement, VeryVerbose, TEXT("No target mesh specified. (%s:%s)"), *GetOwner()->GetName(), *GetName());
		return;
	}
	#endif

	for (uint8 i = 0u; i < static_cast<uint8>(EOculusXREye::COUNT); ++i)
	{
		if (PerEyeData[i].EyeIsMapped)
		{
			const auto& Bone = PerEyeData[i].MappedBoneName;
#if WITH_EYETRACKING_POSEABLEMESH
			int32 BoneIndex = TargetPoseableMeshComponent->GetBoneIndex(Bone);
			FTransform CurrentTransform = TargetPoseableMeshComponent->GetBoneTransformByName(Bone, EBoneSpaces::ComponentSpace);

			CurrentTransform.SetRotation(PerEyeData[i].InitialRotation);

			TargetPoseableMeshComponent->SetBoneTransformByName(Bone, CurrentTransform, EBoneSpaces::ComponentSpace);
#endif
		}
	}
}

bool UGateXREyeTrackingComponent::InitializeEyes()
{
	bool bIsAnythingMapped = false;
#if WITH_EYETRACKING_POSEABLEMESH
	TargetPoseableMeshComponent = OculusXRUtility::FindComponentByName<UPoseableMeshComponent>(GetOwner(), TargetMeshComponentName);

	if (!IsValid(TargetPoseableMeshComponent))
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Could not find mesh with name (%s) for component. (%s:%s)"), *TargetMeshComponentName.ToString(), *GetOwner()->GetName(), *GetName());
		return false;
	}
#endif

    USkinnedMeshComponent* TargetMeshComponent = OculusXRUtility::FindComponentByName<USkinnedMeshComponent>(GetOwner(), TargetMeshComponentName);
	if (!IsValid(TargetMeshComponent)) {
		UE_LOG(LogGateXRMovement, Warning, TEXT("Could not find mesh with name (%s) for component. (%s:%s)"), *TargetMeshComponentName.ToString(), *GetOwner()->GetName(), *GetName());
		return false;
	}

	for (uint8 i = 0u; i < static_cast<uint8>(EOculusXREye::COUNT); ++i)
	{
		const EOculusXREye Eye = static_cast<EOculusXREye>(i);
		const FName* BoneNameForThisEye = EyeToBone.Find(Eye);
		PerEyeData[i].EyeIsMapped = (nullptr != BoneNameForThisEye);

		if (PerEyeData[i].EyeIsMapped)
		{
			UE_LOG(LogGateXRMovement, Warning, TEXT("Eye name is (%s) in mesh %s. (%s:%s)"), *BoneNameForThisEye->ToString(), *TargetMeshComponent->GetName(), *GetOwner()->GetName(), *GetName());

            int32 BoneIndex = TargetMeshComponent->GetBoneIndex(*BoneNameForThisEye);
			if (BoneIndex == INDEX_NONE) {
				PerEyeData[i].EyeIsMapped = false; // Eye is explicitly mapped to a bone. But the bone name doesn't exist.
				UE_LOG(LogGateXRMovement, Warning, TEXT("Could not find bone by name (%s) in mesh %s. (%s:%s)"), *BoneNameForThisEye->ToString(), *TargetMeshComponent->GetName(), *GetOwner()->GetName(), *GetName());
			}
			else {
				PerEyeData[i].MappedBoneName = *BoneNameForThisEye;
     			PerEyeData[i].InitialRotation = TargetMeshComponent->GetBoneQuaternion(*BoneNameForThisEye,EBoneSpaces::ComponentSpace);
				bIsAnythingMapped = true;
			}
		
			#if WITH_EYETRACKING_POSEABLEMESH
			BoneIndex = TargetPoseableMeshComponent->GetBoneIndex(*BoneNameForThisEye);
			if (BoneIndex == INDEX_NONE)
			{
				PerEyeData[i].EyeIsMapped = false; // Eye is explicitly mapped to a bone. But the bone name doesn't exist.
				UE_LOG(LogGateXRMovement, Warning, TEXT("Could not find bone by name (%s) in mesh %s. (%s:%s)"), *BoneNameForThisEye->ToString(), *TargetPoseableMeshComponent->GetName(), *GetOwner()->GetName(), *GetName());
			}
			else
			{
				PerEyeData[i].MappedBoneName = *BoneNameForThisEye;
				PerEyeData[i].InitialRotation = TargetPoseableMeshComponent->GetBoneTransformByName(*BoneNameForThisEye, EBoneSpaces::ComponentSpace).GetRotation();
				bIsAnythingMapped = true;
			}
			#endif
		}
		else
		{
			#if WITH_EYETRACKING_POSEABLEMESH
			UE_LOG(LogGateXRMovement, Display, TEXT("Eye (%s) is not mapped to any bone on mesh (%s)"), *StaticEnum<EOculusXREye>()->GetValueAsString(Eye), *TargetPoseableMeshComponent->GetName());
			#else
			UE_LOG(LogGateXRMovement, Display, TEXT("Eye (%s) is not mapped to any bone on mesh (%s)"), *StaticEnum<EOculusXREye>()->GetValueAsString(Eye), *TargetMeshComponent->GetName());
			#endif
		}
	}

	if (!bIsAnythingMapped)
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Component name -- %s:%s, doesn't have a valid configuration."), *GetOwner()->GetName(), *GetName());
	}

	if (!OculusXRHMD::GetUnitScaleFactorFromSettings(GetWorld(), WorldToMeters))
	{
		UE_LOG(LogGateXRMovement, Warning, TEXT("Cannot get world settings. (%s:%s)"), *GetOwner()->GetName(), *GetName());
	}

	return bIsAnythingMapped;
}
