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

	// ���߼���Լ�Ԥ������
	void SetPredictFootOffsetWithRotator(FName FootAnimCurveName, FVector Velocity, FVector& RootStartLocation, FVector& RootEndLocation,
		FVector& StartLocation, FVector& EndLocation, TArray<FVector>& TransitionPoints, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector, FRotator& FootRotatorTarget);

	// ���������Ϣ
	void UpdatePredictIKData(float FootUpTime, FVector& Root_Start_Location, FVector& Foot_Start_Location, TArray<FVector>& TransitionPoints, FVector Foot_End_Location);

	// ���ɵ���
	bool TraceTransitionPoints(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	// ���ɵ����
	void TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	/*��������Ĵ�ֱƫ��*/
	void SetPelvisOffset(FVector TraceStartLoc, FVector FootEndLoc, FVector FootLandedLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float& PelvisOffset, float TraceTime);

	/*���ýŲ�IK*/
	void SetFootIKOffset(FPredictFootIK& IKValues, FName IKFoot_L, FName IKFoot_R, FAminGraphApplyValue& AnimValues);
	FVector CalculateFootLocAtTime(FVector FootStartLoc, FVector FootEndLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float FootUpTime);

	bool VerticalTraceHit(const FVector& TraceLocation);

	/*���ýŲ������תƫ��*/
	FRotator SetFootIKRotatorOffset(FVector HitNormal);

	/*�Ų���תƫ�Ʋ�ֵ*/
	void SetFootRotatorInterp(FRotator& FootRotatorOffset, const FRotator& FootRotatorTarget);

	/*���㵱ǰʱ�䣨��ǰ�Ľż�����ص�ʱ�䣩�����߻��*/
	void CalculateCurveBlend(FName FootAnimCurveName);

	void NodeBlend(const FBlendNode& BlendNode);

	void HipsBlend(float NodeBlendRate, float VelocityBlendRate, const EPredictHipsDirection& HipsDirection);

	void FinalBlend(float PBlendRate, float NodeBlendRate, float VelocityBlendRate, const FAnimOuterCurves& OuterCurves);

	void UpdateBlendRate();

	// ��ʾ����
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PredictIK| Function")
		void DrawTrace();
	void DrawTrace_Implementation();

protected:

	/* �Ų��������� λ������ */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource WalkCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource RunCurves;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
	FBlendedCurveValue CurrentBlendCurveValue;

	/*PredictIK ������Ϣ*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
		FPredictFootIK PredictFootIKValues;

	// ����������Ϣ
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
		FPredictIKConfig PredictConfig;

	/*Ӧ�ö���ͼIK������Ϣ*/
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
