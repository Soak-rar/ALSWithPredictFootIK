// Fill out your copyright notice in the Description page of Project Settings.


#include "CurveVectorModifier.h"

void UCurveVectorModifier::AddKeyFrameForCurveFloat(UCurveFloat* Curve, float time, float value)
{
	if (Curve)
	{
		Curve->FloatCurve.AddKey(time, value);
	}
}

void UCurveVectorModifier::AddKeyFrameForCurveVector(UCurveVector* Curve, float time, FVector value)
{
	if (Curve)
	{
		Curve->FloatCurves[0].AddKey(time, value.X);
		Curve->FloatCurves[1].AddKey(time, value.Y);
		Curve->FloatCurves[2].AddKey(time, value.Z);
	}

}

void UCurveVectorModifier::AddKeyFrameForCurveRotator(UCurveVector* Curve, float time, FRotator value)
{
	if (Curve)
	{

		Curve->FloatCurves[0].AddKey(time, value.Pitch);
		Curve->FloatCurves[1].AddKey(time, value.Yaw);
		Curve->FloatCurves[2].AddKey(time, value.Roll);
	}
}

void UCurveVectorModifier::ClearKeyCurveVector(UCurveVector* Curve)
{
	if (Curve)
	{
		Curve->FloatCurves[0].Reset();
		Curve->FloatCurves[1].Reset();
		Curve->FloatCurves[2].Reset();
	}
}