// Fill out your copyright notice in the Description page of Project Settings.


#include "CurvesInterface.h"

// Add default functionality here for any ICurvesInterface functions that are not pure virtual.

void ICurvesInterface::AddKeyForCurves_Implementation(UCurveFloat* Curve, float time, float value)
{
	if (Curve)
	{
		Curve->FloatCurve.AddKey(time, value);
	}
}

