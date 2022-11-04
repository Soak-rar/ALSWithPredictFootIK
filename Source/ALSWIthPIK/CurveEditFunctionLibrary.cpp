// Fill out your copyright notice in the Description page of Project Settings.


#include "CurveEditFunctionLibrary.h"


void UCurveEditFunctionLibrary::AddKeyFrameForCurveFloat(UCurveFloat* Curve, float time, float value)
{
	if (Curve)
	{
		Curve->FloatCurve.AddKey(time, value);
	}
}

void UCurveEditFunctionLibrary::AddKeyFrameForCurveVector(UCurveVector* Curve, float time, FVector value)
{
	if (Curve)
	{
		Curve->FloatCurves[0].AddKey(time, value.X);
		Curve->FloatCurves[1].AddKey(time, value.Y);
		Curve->FloatCurves[2].AddKey(time, value.Z);
	}

}

void UCurveEditFunctionLibrary::AddKeyFrameForCurveRotator(UCurveVector* Curve, float time, FRotator value)
{
	if (Curve)
	{

		Curve->FloatCurves[0].AddKey(time, value.Pitch);
		Curve->FloatCurves[1].AddKey(time, value.Yaw);
		Curve->FloatCurves[2].AddKey(time, value.Roll);
	}
}

void UCurveEditFunctionLibrary::ClearKeyCurveVector(UCurveVector* Curve)
{
	if (Curve)
	{
		Curve->FloatCurves[0].Reset();
		Curve->FloatCurves[1].Reset();
		Curve->FloatCurves[2].Reset();
	}
}

void UCurveEditFunctionLibrary::AddFloatCurveKeyForAnimSequence(UAnimSequence* AnimationSequence, FName CurveName, const float Time, const float Value)
{
	const FName ContainerName = UAnimationBlueprintLibrary::RetrieveContainerNameForCurve(AnimationSequence, CurveName);

	if (ContainerName != NAME_None)
	{
		// Retrieve smart name for curve
		const FSmartName CurveSmartName = UAnimationBlueprintLibrary::RetrieveSmartNameForCurve(AnimationSequence, CurveName, ContainerName);

		// Retrieve the curve by name
		FFloatCurve* Curve = static_cast<FFloatCurve*>(AnimationSequence->RawCurveData.GetCurveData(CurveSmartName.UID, ERawCurveTrackTypes::RCT_Float));
		if (Curve)
		{
			
			Curve->FloatCurve.AddKey(Time, Value);
			AnimationSequence->BakeTrackCurvesToRawAnimation();
		}
	}
}
