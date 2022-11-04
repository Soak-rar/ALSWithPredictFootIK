// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "CurveObject.h"
#include "Curves/CurveVector.h"
#include "CurveVectorModifier.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Experimental, config = Editor, defaultconfig)
class ALSWITHPIK_API UCurveVectorModifier : public UAnimationModifier
{
	GENERATED_BODY()

public:

	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence);
	
};
