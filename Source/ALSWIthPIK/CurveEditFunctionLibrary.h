// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Editor/AnimationModifiers/Public/AnimationBlueprintLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CurveEditFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ALSWITHPIK_API UCurveEditFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "ALS|Curve Utils")
	static void AddKeyFrameForCurveFloat(UCurveFloat* Curve, float time, float value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Curve Utils")
	static void AddKeyFrameForCurveVector(UCurveVector* Curve, float time, FVector value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Curve Utils")
	static void AddKeyFrameForCurveRotator(UCurveVector* Curve, float time, FRotator value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Curve Utils")
	static void ClearKeyCurveVector(UCurveVector* Curve);

	UFUNCTION(BlueprintCallable, Category = "ALS|Curve Utils")
	static void AddFloatCurveKeyForAnimSequence(UAnimSequence* AnimationSequence, FName CurveName, const float Time, const float Value);
};
