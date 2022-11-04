// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimEnums.h"
#include "Animation/AnimCurveTypes.h"
#include "AnimationBlueprintLibrary.h"
#include "UObject/Interface.h"
#include "Curves/CurveVector.h"
#include "CurvesInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCurvesInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSWITHPIK_API ICurvesInterface
{
	GENERATED_BODY()
protected:

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddKeyForCurves(UCurveFloat* Curve, float time, float value);
	void AddKeyForCurves_Implementation(UCurveFloat* Curve, float time, float value);

		
public:
};
