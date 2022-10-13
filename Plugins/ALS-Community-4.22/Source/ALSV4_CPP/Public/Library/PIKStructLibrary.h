// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Library/ALSCharacterEnumLibrary.h"

#include "PIKStructLibrary.generated.h"

/**
 *
 */
class UCurveVector;
class UCurveFloat;

UENUM(BlueprintType)
enum class EBaseState : uint8
{
	None,
	Walk,
	Run,
	Crouch

};

UENUM(BlueprintType)
enum class EPredictHipsDirection : uint8
{
	None,
	F,
	B,
	RF,
	RB,
	LF,
	LB
};

USTRUCT(BlueprintType)
struct FPredictFootIK
{
	GENERATED_BODY()

		UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector Foot_L_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector Foot_R_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector Foot_L_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector Foot_R_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		TArray<FVector> Foot_L_Transition_Points;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		TArray<FVector> Foot_R_Transition_Points;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector L_Root_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector R_Root_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector L_Root_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector R_Root_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator L_FootRotator_Target;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator R_FootRotator_Target;
};


USTRUCT(BlueprintType)
struct FAminGraphApplyValue
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator L_FootRotator_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator R_FootRotator_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		float L_FootIK_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		float R_FootIK_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		float PelvisOffset = 0.0f;

	void InitValue()
	{
		L_FootRotator_Offset = FRotator::ZeroRotator;
		R_FootRotator_Offset = FRotator::ZeroRotator;

		L_FootIK_Offset = 0.0f;
		R_FootIK_Offset = 0.0f;
		PelvisOffset = 0.0f;
	}
};

USTRUCT(BlueprintType)
struct FBlendNode
{
	GENERATED_BODY()
		
	EPredictHipsDirection F = EPredictHipsDirection::F;
	EPredictHipsDirection B = EPredictHipsDirection::B;
	EPredictHipsDirection L;
	EPredictHipsDirection R;

	FBlendNode()
	{

	}

	FBlendNode(EPredictHipsDirection L, EPredictHipsDirection R, FName StateName)
	{
		this->L = L;
		this->R = R;
		this->StateName = StateName;
	}

	void SetWeight(UAnimInstance& InAnimInstance, int32 MachineIndex) const
	{
		IsTransition = false;
		float Rate = (StateIndex!=-1 && MachineIndex !=-1)?InAnimInstance.GetInstanceStateWeight(MachineIndex, StateIndex) : 0.f;
		if (Rate != BlendRate && Rate == 1.0f)
		{
			IsTransition = true;
			
		}
		if (Rate != BlendRate && BlendRate == 1.0f)
		{
			IsTransition = true;
		}
		BlendRate = Rate;
	}

	mutable bool IsTransition = false;
	FName StateName;
	mutable int32 StateIndex = -1;
	mutable float BlendRate = 0.0f;
};

USTRUCT(BlueprintType)
struct FAnimOuterCurves
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
	UCurveVector* L_Foot_Vector = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
		UCurveVector* R_Foot_Vector = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
		UCurveVector* L_Ball_Vector = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
		UCurveVector* R_Ball_Vector = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
		UCurveVector* L_Foot_Rotator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Bone Vector Curve")
		UCurveVector* R_Foot_Rotator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float Foot_Up_Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float AnimationLength;

	/*落脚点脚踝节点与根节点的高度差*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float Foot_Root_Landed_Height;
};

USTRUCT(BlueprintType)
struct FCurvesResource
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		TMap<EPredictHipsDirection, FAnimOuterCurves> Curves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float Foot_Up_Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float AnimationLength;

	/*落脚点脚踝节点与根节点的高度差*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		float Foot_Root_Landed_Height;
	 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		EBaseState State;
};

USTRUCT(BlueprintType)
struct FBlendedCurveValue
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector L_Foot_Vector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector R_Foot_Vector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector L_Ball_Vector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector R_Ball_Vector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector L_Foot_Rotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
	FVector R_Foot_Rotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
	float Foot_Up_Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
	float AnimationLength;

	/*落脚点脚踝节点与根节点的高度差*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
	float Foot_Root_Landed_Height;

	void InitVector()
	{
		L_Foot_Vector = FVector::ZeroVector;
		R_Foot_Vector = FVector::ZeroVector;
		L_Ball_Vector = FVector::ZeroVector;
		R_Ball_Vector = FVector::ZeroVector;
		L_Foot_Rotator = FVector::ZeroVector;
		R_Foot_Rotator = FVector::ZeroVector;
	}
};

USTRUCT(BlueprintType)
struct FPredictIKConfig
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
	//FVector Foot_Box_Trace_Half_Size = FVector(5.0f, 8.0f, 14.0f);
	FVector Foot_Box_Trace_Half_Size = FVector(13.0f, 15.0f, 5.0f);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float Vertical_Trace_Height = 60.0f;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float Transition_Height_Threshold = 60.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		int32 Transition_SubSection_Length = 16;

	float FootHeightModify = 16.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
	float Character_Height = 180.0f;

	UPROPERTY(VisibleDefaultsOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
	FVector EnableTrace_Height = FVector(0, 0, 99999.0f);

	/*脚部Ik与预测IK之间的过渡时间*/
	UPROPERTY(VisibleDefaultsOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float MaxTransTime = 0.4f;
};
