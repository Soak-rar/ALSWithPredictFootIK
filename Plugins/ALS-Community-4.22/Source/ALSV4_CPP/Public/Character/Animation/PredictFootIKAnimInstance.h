// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSBaseCharacter.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"

#include "Library/PIKStructLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"

#include "PredictFootIKAnimInstance.generated.h"


class UCurveFloat;
class UAnimSequence;
class UCurveVector;
/**
 * 
 */
UCLASS()
class ALSV4_CPP_API UPredictFootIKAnimInstance : public UALSCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void UpdatePredictFootIK(float DeltaSeconds) override;

public:

	void InitBlendNode();

	// 射线检测以及预测点计算
	void SetPredictFootOffsetWithRotator(FName FootAnimCurveName, FVector Velocity, FVector& RootStartLocation, FVector& RootEndLocation,
		FVector& StartLocation, FVector& EndLocation, TArray<FVector>& TransitionPoints, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector, FRotator& FootRotatorTarget);

	// 更新起点信息
	void UpdatePredictIKData(float FootUpTime, FVector& Root_Start_Location, FVector& Foot_Start_Location, TArray<FVector>& TransitionPoints, FVector Foot_End_Location);

	// 过渡点检测
	bool TraceTransitionPoints(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	// 过渡点过滤
	void TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	/*设置身体的垂直偏移*/
	void SetPelvisOffset(FVector TraceStartLoc, FVector FootEndLoc, FVector FootLandedLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float& PelvisOffset, float TraceTime);

	/*设置脚部IK*/
	void SetFootIKOffset(FPredictFootIK& IKValues, FName IKFoot_L, FName IKFoot_R, FAminGraphApplyValue& AnimValues);
	FVector CalculateFootLocAtTime(FVector FootStartLoc, FVector FootEndLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float FootUpTime);

	bool VerticalTraceHit(const FVector& TraceLocation);

	/*设置脚部落地旋转偏移*/
	FRotator SetFootIKRotatorOffset(FVector HitNormal);

	/*脚部旋转偏移插值*/
	void SetFootRotatorInterp(FRotator& FootRotatorOffset, const FRotator& FootRotatorTarget);

	/*计算当前时间（当前的脚即将落地的时间）的曲线混合*/
	void CalculateCurveBlend(FName FootAnimCurveName);

	void NodeBlend(const FBlendNode& BlendNode);

	void HipsBlend(float NodeBlendRate, float VelocityBlendRate, const EPredictHipsDirection& HipsDirection);

	void FinalBlend(float PBlendRate, float NodeBlendRate, float VelocityBlendRate, const FAnimOuterCurves& OuterCurves);

	void UpdateBlendRate();

	// 显示绘制
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PredictIK| Function")
		void DrawTrace();
	void DrawTrace_Implementation();

protected:

	/* 脚部动画序列 位置曲线 */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource WalkCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource RunCurves;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
	FBlendedCurveValue CurrentBlendCurveValue;

	/*PredictIK 数据信息*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
		FPredictFootIK PredictFootIKValues;

	// 基础配置信息
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
		FPredictIKConfig PredictConfig;

	/*应用动画图IK数据信息*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
		FAminGraphApplyValue AminGraphApplyValues;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Foot IK", Meta = (
		ShowOnlyInnerProperties))
		FVector Speed;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
		bool IsNodeChanged = false;
private:
	float DeltaTime;

	int32 MachineIndex = -1;

	EBaseState CurrentState = EBaseState::None;

	bool IsApplyFootIK = true;

	TArray<FBlendNode> BlendNodeArray;

	float TransitionTime = 0.0f;

	float LockedPIK;

	float LockedLIK;

	float LockedRIK;

	FName StateMachineName = FName("(N) Directional States");

	float LUpTime;

	float RUpTime;
};
