// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Curves/CurveVector.h"
#include "CurveVectorModifier.generated.h"

/**
 * 
 */
UCLASS()
class ALSWITHPIK_API UCurveVectorModifier : public UAnimationModifier
{
	GENERATED_BODY()

		friend class FAnimationModifierDetailCustomization;
public:
	UFUNCTION(BlueprintCallable, Category = "Curve Edit")
		void AddKeyFrameForCurveFloat(UCurveFloat* Curve, float time, float value);

	UFUNCTION(BlueprintCallable, Category = "Curve Edit")
		void AddKeyFrameForCurveVector(UCurveVector* Curve, float time, FVector value);

	UFUNCTION(BlueprintCallable, Category = "Curve Edit")
		void AddKeyFrameForCurveRotator(UCurveVector* Curve, float time, FRotator value);

	UFUNCTION(BlueprintCallable, Category = "Curve Edit")
		void ClearKeyCurveVector(UCurveVector* Curve);
	
};
