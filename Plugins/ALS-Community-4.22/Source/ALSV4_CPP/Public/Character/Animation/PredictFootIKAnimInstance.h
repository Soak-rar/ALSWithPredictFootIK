// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSBaseCharacter.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "Kismet/KismetStringLibrary.h"
#include "Library/PIKStructLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
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

	/*在 ALSCharacterAnimInstance中调用的虚函数*/
	virtual void UpdatePredictFootIK(float DeltaSeconds) override;

public:
	/*初始化六向 混合的状态节点， 每个状态节点中对应 四种 混合姿态*/
	void InitBlendNode();

	void IsUsePredictIK();

	// 射线检测以及预测点计算
	void SetPredictFootOffsetWithRotator(float LeaderFootUpTime, FVector Velocity, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector, FPredictFootValue& FootValue);

	// 更新起点信息
	void UpdatePredictIKData(float FootUpTime, FPredictFootValue& FootValue);

	// 过渡点检测
	bool TraceTransitionPoints(FPredictFootValue& FootValue);

	// 过渡点过滤
	void TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	/*设置身体的垂直偏移*/
	void SetPelvisOffset(FVector FootLandedLoc, float Time, FVector& PelvisOffset, float TraceTime, FName FootName, FPredictFootValue& FootValue, FName OtherName);

	/*设置脚部IK*/
	void SetFootIKOffset();

	/*通过当前时间 计算在预测路径中对应的位置*/
	FVector CalculateFootLocAtTime(float Time, float FootUpTime, const FPredictFootValue& FootValue, bool Apply);

	/*过滤初始碰撞、不可抵达、高点检测*/
	bool VerticalTraceHit(const FVector& TraceLocation);

	/*设置脚部落地旋转偏移*/
	FRotator SetFootIKRotatorOffset(FVector HitNormal);

	/*脚部旋转偏移插值*/
	void SetFootRotatorInterp(FRotator& FootRotatorOffset, const FRotator& FootRotatorTarget);

	/*计算当前时间（当前的脚即将落地的时间）的曲线混合*/
	void CalculateCurveBlend(FName FootAnimCurveName);

	/*更新 六向状态节点的混合率*/
	void UpdateBlendRate();

	/*更新 所有动画 的混合率*/
	void UpdateCurvesBlendRate(FCurvesResource& Curves, float PoseBlendRate, float StrideBlendRate);
	void UpdateEachFinalBlend(float PBlendRate, float StrideBlendRate, float NodeBlendRate, float VelocityBlendRate, EPredictHipsDirection& HipsDirection, FCurvesResource& CurvesResource);

	/*更新领导者动画信息*/
	void UpdateLeaderInfo();

	/*由于得到的是多混合后的曲线， 但是需要的是单独的曲线值。目前已知 领导者 与各跟随者的 换算公式， 则可以计算出领导者当前的 CurrentAnimTime和 UpFootTime*/
	void CalculateLeaderValue();

	/*计算公式的 推导每个独立动画序列曲线值的函数系数*/
	float CalculateTrsaitionValue();

	/*混合 每个参与混合的动画序列的单独信息，得到预测的原始 相对于root节点位置*/
	void CalculateApplyBlend(FCurvesResource& CurvesResource);

	void InitAnimBlendRate(FCurvesResource& CurvesResource);

	float CalculateVectorAngle(const FVector& Vector_1, const FVector& Vector_2);

	/*检测 落脚点射线物体的 移动变化，该变化将应用到固定值上（如已经落地脚的 落脚点， 抬脚的起点）*/
	void AdditionalOffset();

	// 显示绘制
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PredictIK| Function")
		void DrawTrace();
	void DrawTrace_Implementation();

	bool ApplyPredictIK();


protected:

	/* 脚部动画序列 曲线资源 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource WalkCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource RunCurves;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
	FBlendedCurveValue CurrentBlendCurveValue;

	/*预测过程中 左右脚 的信息*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Predict IK - Intermediate data", Meta = (ShowOnlyInnerProperties))
	FPredictFootValue LeftFootValue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Predict IK - Intermediate data", Meta = (ShowOnlyInnerProperties))
	FPredictFootValue RightFootValue;

	// 基础配置信息
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (ShowOnlyInnerProperties))
		FPredictIKConfig PredictConfig;

	/*应用动画图IK数据信息*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (ShowOnlyInnerProperties))
		FAminGraphApplyValue AminGraphApplyValues;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Foot IK", Meta = (ShowOnlyInnerProperties))
		FVector Speed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
	FLeaderAnimInfo Leader;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
		bool IsUseTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
		bool IsApplyPredictSwitch = true;
	
private:
	float DeltaTime;

	bool IsApplyPredictIK = false;

	bool IsApplyFootIK = false;

	bool IsApplyInterp = false;

	float InitInpterpTime = 0.0f;

	float InterpSpeed = 10.0f;

	FString HitActorName;

	FVector HitActorLocation = FVector::ZeroVector;

	FVector FootOffset = FVector::ZeroVector;

	UPrimitiveComponent* HitComponent;

	/*记录ALS 状态机Directional States 的六种状态的混合程度，以及状态内部的 四种动画的混合权重*/
	TArray<FBlendNode> BlendNodeArray;

	/*获取ALS状态机的混合信息*/
	FName StateMachineName = FName("(N) Directional States");
	
	int32 MachineIndex = -1;

	/*动画混合的 抬脚曲线值*/
	float LUpTime;

	float RUpTime;

	float TransitionThreshold = 0.005f;
};
