#include "GateXRMorphTargetsController.h"


void FGateXRMorphTargetsController::SetMorphTarget(FName MorphTargetName, float Value)
{
	float* CurveValPtr = MorphTargetCurves.Find(MorphTargetName);
	bool bShouldAddToList = FPlatformMath::Abs(Value) > ZERO_ANIMWEIGHT_THRESH;
	if (bShouldAddToList)
	{
		if (CurveValPtr)
		{
			// sum up, in the future we might normalize, but for now this just sums up
			// this won't work well if all of them have full weight - i.e. additive
			*CurveValPtr = Value;
		}
		else
		{
			MorphTargetCurves.Add(MorphTargetName, Value);
		}
	}
	// if less than ZERO_ANIMWEIGHT_THRESH
	// no reason to keep them on the list
	else
	{
		// remove if found
		MorphTargetCurves.Remove(MorphTargetName);
	}
}

float FGateXRMorphTargetsController::GetMorphTarget(FName MorphTargetName) const
{
	const float* CurveValPtr = MorphTargetCurves.Find(MorphTargetName);

	if (CurveValPtr)
	{
		return *CurveValPtr;
	}
	else
	{
		return 0.0f;
	}
}

void FGateXRMorphTargetsController::ClearMorphTargets()
{
	MorphTargetCurves.Empty();
}
