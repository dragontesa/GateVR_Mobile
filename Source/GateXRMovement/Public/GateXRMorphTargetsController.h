#pragma once


class GATEXRMOVEMENT_API FGateXRMorphTargetsController
{
    public:
	
	// Sets a specific morph target value
	void SetMorphTarget(FName MorphTargetName, float Value);

	// Gets a specific morph target value
	float GetMorphTarget(FName MorphTargetName) const;

	// Clears all morph target curves data
	void ClearMorphTargets();

	// List of morph targets on this controller
	TMap<FName, float> MorphTargetCurves;
};