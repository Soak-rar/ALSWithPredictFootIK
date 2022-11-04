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


/*动画序列的类别*/
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

/*预测过程中的信息*/
USTRUCT(BlueprintType)
struct FPredictFootValue
{
	GENERATED_BODY()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	FVector Foot_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	FVector Foot_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	TArray<FVector> Foot_Transition_Points;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	FVector Root_Start_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	FVector Root_End_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
	FRotator FootRotator_Target;
};

/*记录实际应用到 动画图的 脚部IK值*/
USTRUCT(BlueprintType)
struct FAminGraphApplyValue
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator L_FootRotator_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK AminGraph Value")
		FRotator R_FootRotator_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector L_FootIK_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector R_FootIK_Offset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
		FVector PelvisOffset = FVector::ZeroVector;

	void InitValue()
	{
		L_FootRotator_Offset = FRotator::ZeroRotator;
		R_FootRotator_Offset = FRotator::ZeroRotator;

		L_FootIK_Offset = FVector::ZeroVector;
		R_FootIK_Offset = FVector::ZeroVector;
		PelvisOffset = FVector::ZeroVector;
	}
};

/*对应ALS大环的 六向状态混合，每个状态节点对应一个FBlendNode*/
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

/*每个动画序列对应的曲线资源以及相关信息*/
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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Values")
	float FinalBlendRate = 0.0f;
};

/*记录 每个动画序列的曲线资源 信息*/

USTRUCT(BlueprintType)
struct FCurvesResource
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
		TMap<EPredictHipsDirection, FAnimOuterCurves> Curves;
};

/*记录由ALS混合动画图混合后的 动画参数， 以及 动画同步组 领导者的 动画时长、 抬脚时长*/
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

	/*混合度最高的动画（同步组领导者）抬脚时长*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
	float LeaderUpFootTime;

	/*混合度最高的动画（同步组领导者）动画时长*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Anim Time Info")
	float LeaderAnimLength;

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
		LeaderUpFootTime = 0.0f;
		LeaderAnimLength = 0.0f;
		Foot_Root_Landed_Height = 15.5f;
	}
};

/*一些配置固定参数*/
USTRUCT(BlueprintType)
struct FPredictIKConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ALS|Anim Graph - Predict Foot IK Config")
	//FVector Foot_Box_Trace_Half_Size = FVector(5.0f, 8.0f, 14.0f);
	FVector Foot_Box_Trace_Half_Size = FVector(13.0f, 15.0f, 5.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float Vertical_Trace_Height = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float Transition_Height_Threshold = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
		int32 Transition_SubSection_Length = 16;

	float FootHeightModify = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Config")
	float Character_Height = 180.0f;

	UPROPERTY(EditAnywhere, Category = "ALS|Anim Graph - Predict Foot IK Config")
	FVector EnableTrace_Height = FVector(0, 0, 99999.0f);

	/*脚部Ik与预测IK之间的过渡时间*/
	UPROPERTY(EditAnywhere, Category = "ALS|Anim Graph - Predict Foot IK Config")
		float MaxTransTime = 0.2f;
};

/*混合后领导者 动画 的单独动画曲线信息（由混合值计算而来）*/
USTRUCT(BlueprintType)
struct FLeaderAnimInfo
{
	GENERATED_BODY()
		UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
		float LFootUpTime;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
		float RFootUpTime;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
		float CurrentAnimTime;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Predict Foot IK Value")
		EPredictHipsDirection LeaderAnimType;

	void InitValue()
	{
		LFootUpTime = 0.0f;
		RFootUpTime = 0.0f;
		CurrentAnimTime = 0.0f;
		LeaderAnimType = EPredictHipsDirection::None;
	}
};
