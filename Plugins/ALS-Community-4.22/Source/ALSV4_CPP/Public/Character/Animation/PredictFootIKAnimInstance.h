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

	/*�� ALSCharacterAnimInstance�е��õ��麯��*/
	virtual void UpdatePredictFootIK(float DeltaSeconds) override;

public:
	/*��ʼ������ ��ϵ�״̬�ڵ㣬 ÿ��״̬�ڵ��ж�Ӧ ���� �����̬*/
	void InitBlendNode();

	void IsUsePredictIK();

	// ���߼���Լ�Ԥ������
	void SetPredictFootOffsetWithRotator(float LeaderFootUpTime, FVector Velocity, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector, FPredictFootValue& FootValue);

	// ���������Ϣ
	void UpdatePredictIKData(float FootUpTime, FPredictFootValue& FootValue);

	// ���ɵ���
	bool TraceTransitionPoints(FPredictFootValue& FootValue);

	// ���ɵ����
	void TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints);

	/*��������Ĵ�ֱƫ��*/
	void SetPelvisOffset(FVector FootLandedLoc, float Time, FVector& PelvisOffset, float TraceTime, FName FootName, FPredictFootValue& FootValue, FName OtherName);

	/*���ýŲ�IK*/
	void SetFootIKOffset();

	/*ͨ����ǰʱ�� ������Ԥ��·���ж�Ӧ��λ��*/
	FVector CalculateFootLocAtTime(float Time, float FootUpTime, const FPredictFootValue& FootValue, bool Apply);

	/*���˳�ʼ��ײ�����ɵִ�ߵ���*/
	bool VerticalTraceHit(const FVector& TraceLocation);

	/*���ýŲ������תƫ��*/
	FRotator SetFootIKRotatorOffset(FVector HitNormal);

	/*�Ų���תƫ�Ʋ�ֵ*/
	void SetFootRotatorInterp(FRotator& FootRotatorOffset, const FRotator& FootRotatorTarget);

	/*���㵱ǰʱ�䣨��ǰ�Ľż�����ص�ʱ�䣩�����߻��*/
	void CalculateCurveBlend(FName FootAnimCurveName);

	/*���� ����״̬�ڵ�Ļ����*/
	void UpdateBlendRate();

	/*���� ���ж��� �Ļ����*/
	void UpdateCurvesBlendRate(FCurvesResource& Curves, float PoseBlendRate, float StrideBlendRate);
	void UpdateEachFinalBlend(float PBlendRate, float StrideBlendRate, float NodeBlendRate, float VelocityBlendRate, EPredictHipsDirection& HipsDirection, FCurvesResource& CurvesResource);

	/*�����쵼�߶�����Ϣ*/
	void UpdateLeaderInfo();

	/*���ڵõ����Ƕ��Ϻ�����ߣ� ������Ҫ���ǵ���������ֵ��Ŀǰ��֪ �쵼�� ��������ߵ� ���㹫ʽ�� ����Լ�����쵼�ߵ�ǰ�� CurrentAnimTime�� UpFootTime*/
	void CalculateLeaderValue();

	/*���㹫ʽ�� �Ƶ�ÿ������������������ֵ�ĺ���ϵ��*/
	float CalculateTrsaitionValue();

	/*��� ÿ�������ϵĶ������еĵ�����Ϣ���õ�Ԥ���ԭʼ �����root�ڵ�λ��*/
	void CalculateApplyBlend(FCurvesResource& CurvesResource);

	void InitAnimBlendRate(FCurvesResource& CurvesResource);

	float CalculateVectorAngle(const FVector& Vector_1, const FVector& Vector_2);

	/*��� ��ŵ���������� �ƶ��仯���ñ仯��Ӧ�õ��̶�ֵ�ϣ����Ѿ���ؽŵ� ��ŵ㣬 ̧�ŵ���㣩*/
	void AdditionalOffset();

	// ��ʾ����
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PredictIK| Function")
		void DrawTrace();
	void DrawTrace_Implementation();

	bool ApplyPredictIK();


protected:

	/* �Ų��������� ������Դ */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource WalkCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Animation Sequence Struct|Predict Foot IK")
	FCurvesResource RunCurves;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Predict Foot IK")
	FBlendedCurveValue CurrentBlendCurveValue;

	/*Ԥ������� ���ҽ� ����Ϣ*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Predict IK - Intermediate data", Meta = (ShowOnlyInnerProperties))
	FPredictFootValue LeftFootValue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Predict IK - Intermediate data", Meta = (ShowOnlyInnerProperties))
	FPredictFootValue RightFootValue;

	// ����������Ϣ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (ShowOnlyInnerProperties))
		FPredictIKConfig PredictConfig;

	/*Ӧ�ö���ͼIK������Ϣ*/
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

	/*��¼ALS ״̬��Directional States ������״̬�Ļ�ϳ̶ȣ��Լ�״̬�ڲ��� ���ֶ����Ļ��Ȩ��*/
	TArray<FBlendNode> BlendNodeArray;

	/*��ȡALS״̬���Ļ����Ϣ*/
	FName StateMachineName = FName("(N) Directional States");
	
	int32 MachineIndex = -1;

	/*������ϵ� ̧������ֵ*/
	float LUpTime;

	float RUpTime;

	float TransitionThreshold = 0.005f;
};
