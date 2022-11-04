// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/PredictFootIKAnimInstance.h"



static const FName Name_L_Foot_UptoDown(TEXT("L_UptoDown"));
static const FName Name_R_Foot_UptoDown(TEXT("R_UptoDown"));
static const FName Name_CurrentAnimTime(TEXT("CurrentAnimTime"));
static const FName Name_Bone_Root(TEXT("root"));
static const FName Name_Bone_Pelvis(TEXT("Pelvis"));
static const FName Name_IKBone_FootL(TEXT("VB foot_target_l"));
static const FName Name_IKBone_FootR(TEXT("VB foot_target_r"));
static const FName Name_Bone_FootL(TEXT("Foot_L"));
static const FName Name_Bone_FootR(TEXT("Foot_R"));


void UPredictFootIKAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	InitBlendNode();
	
}

void UPredictFootIKAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	LeftFootValue.Foot_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootL);
	RightFootValue.Foot_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootR);

}

void UPredictFootIKAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UPredictFootIKAnimInstance::UpdatePredictFootIK(float DeltaSeconds)
{
	if (!IsApplyPredictSwitch)
	{
		AminGraphApplyValues.PelvisOffset = FootIKValues.PelvisOffset;
		AminGraphApplyValues.L_FootIK_Offset = FootIKValues.FootOffset_L_Location;
		AminGraphApplyValues.R_FootIK_Offset = FootIKValues.FootOffset_R_Location;
		AminGraphApplyValues.L_FootRotator_Offset = FootIKValues.FootOffset_L_Rotation;
		AminGraphApplyValues.R_FootRotator_Offset = FootIKValues.FootOffset_R_Rotation;
		return;
	}

	DeltaTime = DeltaSeconds;
	if (Character)
	{
		UPawnMovementComponent* MovementComponent = Character->GetMovementComponent();
		MovementComponent->GetMaxSpeed();
		Speed = Character->GetVelocity();
	}
	UpdateBlendRate();
	AdditionalOffset();
	LUpTime = GetCurveValue(Name_L_Foot_UptoDown);
	RUpTime = GetCurveValue(Name_R_Foot_UptoDown);

	IsUsePredictIK();

	if(IsApplyPredictIK)
	{
			/*应用预测IK*/
		IsApplyFootIK = ApplyPredictIK();
	}

	if (!IsApplyPredictIK)
	{
		LeftFootValue.Foot_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootL);
		RightFootValue.Foot_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootR);

		UpdatePredictIKData(LUpTime, LeftFootValue);
		UpdatePredictIKData(RUpTime, RightFootValue);

		AminGraphApplyValues.L_FootIK_Offset = UKismetMathLibrary::VInterpTo(AminGraphApplyValues.L_FootIK_Offset, FootLTarget, DeltaSeconds, InterpSpeed);
		AminGraphApplyValues.R_FootIK_Offset = UKismetMathLibrary::VInterpTo(AminGraphApplyValues.R_FootIK_Offset, FootRTarget, DeltaSeconds, InterpSpeed);
		AminGraphApplyValues.PelvisOffset = UKismetMathLibrary::VInterpTo(AminGraphApplyValues.PelvisOffset, PredictPelvisTarget, DeltaSeconds, InterpSpeed);

	}
	
	if (InitInpterpTime >= 0.0f)
	{
		InitInpterpTime -= DeltaSeconds;
	}
	DrawTrace();
}

bool UPredictFootIKAnimInstance::ApplyPredictIK()
{
	CalculateCurveBlend(Name_L_Foot_UptoDown);

	UpdatePredictIKData(LUpTime, LeftFootValue);
	UpdatePredictIKData(RUpTime, RightFootValue);
	if (LUpTime > TransitionThreshold)
	{
		SetPredictFootOffsetWithRotator(Leader.LFootUpTime, Speed, CurrentBlendCurveValue.L_Foot_Vector, CurrentBlendCurveValue.L_Foot_Rotator, CurrentBlendCurveValue.L_Ball_Vector, LeftFootValue);

		LeftFootValue.Foot_Start_Location += FootOffset;


		TraceTransitionPoints(LeftFootValue);
	}
	else if (RUpTime > TransitionThreshold)
	{
		SetPredictFootOffsetWithRotator(Leader.RFootUpTime, Speed, CurrentBlendCurveValue.R_Foot_Vector, CurrentBlendCurveValue.R_Foot_Rotator, CurrentBlendCurveValue.R_Ball_Vector, RightFootValue);


		RightFootValue.Foot_Start_Location += FootOffset;

		TraceTransitionPoints(RightFootValue);
	}

	if (!IsApplyPredictIK)
	{
		return false;
	}

	SetFootIKOffset();

	//UKismetMathLibrary::Acos();

	if (LUpTime > TransitionThreshold)
	{
		SetFootRotatorInterp(AminGraphApplyValues.L_FootRotator_Offset, LeftFootValue.FootRotator_Target);
		SetPelvisOffset(RightFootValue.Foot_End_Location, Leader.LFootUpTime, AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.LeaderUpFootTime, Name_IKBone_FootL, LeftFootValue, Name_IKBone_FootR);
	}
	else if (RUpTime > TransitionThreshold)
	{
		SetFootRotatorInterp(AminGraphApplyValues.R_FootRotator_Offset, RightFootValue.FootRotator_Target);
		SetPelvisOffset(LeftFootValue.Foot_End_Location, Leader.RFootUpTime, AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.LeaderUpFootTime, Name_IKBone_FootR, RightFootValue, Name_IKBone_FootL);
	}
	return true;
}



void UPredictFootIKAnimInstance::InitBlendNode()
{

	const FBakedAnimationStateMachine* MachineDescription = nullptr;
	GetStateMachineIndexAndDescription(StateMachineName, MachineIndex, &MachineDescription);

	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LF, EPredictHipsDirection::RF, FName("Move F")));
	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LB, EPredictHipsDirection::RB, FName("Move B")));
	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LF, EPredictHipsDirection::RB, FName("Move LF")));
	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LB, EPredictHipsDirection::RF, FName("Move LB")));
	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LB, EPredictHipsDirection::RF, FName("Move RF")));
	BlendNodeArray.Add(FBlendNode(EPredictHipsDirection::LF, EPredictHipsDirection::RB, FName("Move RB")));

	if (MachineDescription)
	{
		for (int32 i = 0; i < BlendNodeArray.Num();i++)
		{
			BlendNodeArray[i].StateIndex = MachineDescription->FindStateIndex(BlendNodeArray[i].StateName);
		}
	}
}

void UPredictFootIKAnimInstance::IsUsePredictIK()
{
	//IsApplyPredictIK = false;
	IsApplyInterp = false;
	float WGait = GetCurveValue(NAME_W_Gait);

	if (WGait <= 0.9999f || (WGait >= 1.0001f && WGait <= 1.9999f) || WGait > 2.0001f)
	{
		IsApplyInterp = true;
	}

	if (MovementState == EALSMovementState::Grounded && WGait > 0.9999f && WGait < 2.0001f)
	{
		if (!IsApplyPredictIK)
		{
			InitInpterpTime = 0.3f;
		}
		IsApplyPredictIK = true;
	}
	else
	{
		IsApplyPredictIK = false;
	}

	if (Stance.Crouching())
	{
		IsApplyPredictIK = false;
	}

}

void UPredictFootIKAnimInstance::SetPredictFootOffsetWithRotator(float LeaderFootUpTime, FVector Velocity, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector, FPredictFootValue& FootValue)
{
	// 计算落脚位置
	FTransform CurrentRootTransform = GetOwningComponent()->GetSocketTransform(Name_Bone_Root);
	FTransform PredictRootTransform = FTransform(CurrentRootTransform.GetRotation(), (LeaderFootUpTime * Velocity/Grounded.StandingPlayRate + CurrentRootTransform.GetLocation()), CurrentRootTransform.GetScale3D());

	FVector FootEndLocation = PredictRootTransform.TransformPosition(FootCurveVector);

	/**/
	FVector PredictBallLocation = PredictRootTransform.TransformPosition(BallCurveVector);

	const FVector TraceCenter = (FootEndLocation * 0.5 + PredictBallLocation * 0.5);

	const FVector FootRotatorVector = FootCurveRotator;

	FQuat FootEndRotator = PredictRootTransform.TransformRotation(FQuat(FRotator::MakeFromEuler(FVector(FootRotatorVector.Z, FootRotatorVector.X, FootRotatorVector.Y))));
	FRotator EndRotator = FRotator::MakeFromEuler(PredictBallLocation - FootEndLocation);
	/**/



	/**/
	const FVector TraceStart = TraceCenter + FVector(0.0, 0.0, PredictConfig.Vertical_Trace_Height);
	const FVector TraceEnd = TraceCenter - FVector(0.0, 0.0, PredictConfig.Vertical_Trace_Height);

	/**/
	EDrawDebugTrace::Type DrawTrace = EDrawDebugTrace::None;

	if (IsUseTrace)
	{
		DrawTrace = EDrawDebugTrace::ForDuration;
	}

	UWorld* World = GetWorld();
	check(World);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	FHitResult HitResult;
	FCollisionShape BoxShape;
	BoxShape.SetBox(PredictConfig.Foot_Box_Trace_Half_Size);
	
	bool bHit = false;
	if (GetWorld())
	{
		bHit = GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FRotator(90, FootEndRotator.Rotator().Yaw, 90).Quaternion(), ECC_Visibility, BoxShape);
	}

	//const bool bHit = UKismetSystemLibrary::BoxTraceSingle(
	//	this, TraceStart, TraceEnd, PredictConfig.Foot_Box_Trace_Half_Size, FRotator(90, FootEndRotator.Rotator().Yaw, 90),
	//	ETraceTypeQuery::TraceTypeQuery_MAX, false, ActorsToIgnore, DrawTrace, HitResult, true,
	//	FLinearColor::Red, FLinearColor::Green, CurrentBlendCurveValue.LeaderAnimLength);

	/*不可达： 初始碰撞， 高点射线 检测*/

	bool bVerticalHit = VerticalTraceHit(FootEndLocation);


	if (bHit && (!HitResult.bStartPenetrating) && (!bVerticalHit))
	{
		FootValue.Foot_End_Location = HitResult.Location + (FootEndLocation - TraceCenter);
	
		FootValue.FootRotator_Target = SetFootIKRotatorOffset(HitResult.ImpactNormal);

		/*修正脚在斜面的 Z位置 待优化*/
		//if (HitResult.ImpactNormal.Z < 1)
		//{
		//	FootValue.Foot_End_Location -= FVector(0, 0, 5);
		//}
		

		UKismetSystemLibrary::DrawDebugPoint(this, FootValue.Foot_End_Location, 4.0f, FLinearColor::Blue);
		//UKismetSystemLibrary::PrintString(this, (HitResult.Location- TraceCenter).ToString());
	}
	else
	{
		//UKismetSystemLibrary::PrintString(this, "false");
		//UKismetSystemLibrary::PrintString(this, (HitResult.Location - TraceCenter).ToString());
		IsApplyPredictIK = false;
		InterpSpeed = 30.0f;
	
		//FootValue.Foot_End_Location = FootEndLocation;
	}
	
}

void UPredictFootIKAnimInstance::UpdatePredictIKData(float FootUpTime, FPredictFootValue& FootValue)
{
	// 根据脚步曲线更新角色起点位置，曲线值 =0 更新，>0 不更新
	if (FootUpTime <= TransitionThreshold)
	{
		if (FootValue.Foot_Transition_Points.Num() != 0)
		{
			FootValue.Foot_Transition_Points.Empty();
		}
		FootValue.Root_Start_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_Root);
		FootValue.Foot_End_Location += FootOffset;
		FootValue.Foot_Start_Location = FootValue.Foot_End_Location;

	}
}

bool UPredictFootIKAnimInstance::TraceTransitionPoints(FPredictFootValue& FootValue)
{
	FVector FootEndLocation = FootValue.Foot_End_Location;
	FVector FootStartLocation = FootValue.Foot_Start_Location;
	FVector SubVector = FootEndLocation - FootStartLocation;
	float LineLength = SubVector.Size();
	TArray<AActor*> ActorsToIgnore;
	FootValue.Foot_Transition_Points.Empty();

	int32 SubSections = UKismetMathLibrary::FFloor(LineLength) / PredictConfig.Transition_SubSection_Length;
	int32 Rest = UKismetMathLibrary::FFloor(LineLength) % PredictConfig.Transition_SubSection_Length;
	float LastSub = Rest * 1.0f / PredictConfig.Transition_SubSection_Length;
	float RatioEachSub = (1 - (LastSub) / (LastSub + SubSections)) / SubSections;
	float TraceEndLocaionZ = FootStartLocation.Z > FootEndLocation.Z ? FootEndLocation.Z : FootStartLocation.Z;
	float TraceStartLocationZ = TraceEndLocaionZ + PredictConfig.Transition_Height_Threshold;
	//TransitionPoints.Add(FootStartLocation);
	for (int32 i = 1; i <= SubSections; i++)
	{
		FHitResult HitResult;
		float SubStartX = (SubVector.X * i - SubVector.X * (i - 1)) * RatioEachSub / 2 + SubVector.X * (i - 1) * RatioEachSub + FootStartLocation.X;
		float SubStartY = (SubVector.Y * i - SubVector.Y * (i - 1)) * RatioEachSub / 2 + SubVector.Y * (i - 1) * RatioEachSub + FootStartLocation.Y;
		float SubStartZ = (SubVector.Z * i - SubVector.Z * (i - 1)) * RatioEachSub / 2 + SubVector.Z * (i - 1) * RatioEachSub + FootStartLocation.Z;
		FVector TraceStartLoc = FVector(SubStartX, SubStartY, TraceStartLocationZ);
		FVector TraceEndLoc = FVector(SubStartX, SubStartY, TraceEndLocaionZ);
		bool bHit = false;
		if (Character)
		{
			bHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStartLoc, TraceEndLoc, FVector(8.0f, 15.0f, 5.0f), Character->GetActorRotation(),
				ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
		}

		if (bHit)
		{
			// 发生初始碰撞的情况，即过渡点的抬脚高度大于设定的阈值，无法实现抬脚动作
			if (HitResult.bStartPenetrating)
			{
				return false;
			}
			// 待优化：检测结果的Location应该 > （当前直线的Z值 + 此时的脚原始抬起高度）
			if (HitResult.Location.Z > SubStartZ)
			{
				/*修正：脚部过渡阶段 脚尖穿模，抬高过渡点*/
				FootValue.Foot_Transition_Points.Add(HitResult.Location + FVector(0, 0, 8));
			}
		}
	}
	// 过滤列表：
	if (FootValue.Foot_Transition_Points.Num() > 0)
	{
		TransitionPointsFilter(FootStartLocation, FootEndLocation, FootValue.Foot_Transition_Points);
	}
	
	//TransitionPoints.Add(FootEndLocation);
	return true;
}

void UPredictFootIKAnimInstance::TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints)
{// 过渡点与起始终点做成的路线应该为 上凸曲线，删除造成下凸的过渡点
	TArray<FVector> Temporary;
	int32 CurrentIndex = -1;
	float MaxAngle = 0.0f; 
	FVector MaxAngleVcetor;
	FVector Vector_1 = FootStartLocation;
	bool IsHasPoint = true;
	float RefAngle;
 

	while (IsHasPoint)
	{
		IsHasPoint = false;
		int32 MaxIndex = -1;
		MaxAngle = 0.0f;
		if (CurrentIndex == -1)
		{
			RefAngle = 0.0f;
		}
		else
		{
			RefAngle = CalculateVectorAngle(FootStartLocation - FootEndLocation, Vector_1 - FootEndLocation);
		}
		for (int32 i = CurrentIndex + 1; i < TransitionPoints.Num(); i++)
		{
			if (CalculateVectorAngle(FootStartLocation - FootEndLocation, TransitionPoints[i] - FootEndLocation) > RefAngle)
			{
				IsHasPoint = true;
				if (CalculateVectorAngle(FootEndLocation - Vector_1, TransitionPoints[i] - Vector_1) > MaxAngle)
				{
					MaxAngleVcetor = TransitionPoints[i];
					MaxIndex = i;
					MaxAngle = CalculateVectorAngle(FootEndLocation - Vector_1, TransitionPoints[i] - Vector_1);
				}
			}
		}

		if (IsHasPoint)
		{
			Temporary.Add(MaxAngleVcetor);
			Vector_1 = MaxAngleVcetor;
			CurrentIndex = MaxIndex;
		}
		
	}

	TransitionPoints = Temporary;
	
}

void UPredictFootIKAnimInstance::SetPelvisOffset(FVector FootLandedLoc, float Time, FVector& PelvisOffset, float TraceTime, FName FootName, FPredictFootValue& FootValue, FName OtherName)
{
	USkeletalMeshComponent* Mesh = GetOwningComponent();

	FVector CurrentFootLoction = CalculateFootLocAtTime(Time, TraceTime, FootValue, false);
	
	FVector PelvisTarget = FVector::ZeroVector;

	if (CurrentFootLoction.Z >= FootLandedLoc.Z)
	{
		PelvisTarget = FootLandedLoc - Mesh->GetSocketLocation(Name_Bone_Root) -CurrentBlendCurveValue.Foot_Root_Landed_Height;

	}
	else
	{
		PelvisTarget = CurrentFootLoction - Mesh->GetSocketLocation(Name_Bone_Root)- CurrentBlendCurveValue.Foot_Root_Landed_Height;
	}

	if (IsApplyInterp || InitInpterpTime >= 0.0f)
	{
		InterpSpeed = InitInpterpTime >= 0.0f ? 30.0f : 20.0f;
		PelvisOffset = UKismetMathLibrary::VInterpTo(PelvisOffset, PelvisTarget, DeltaTime, InterpSpeed);
	}
	else {
		PelvisOffset = PelvisTarget;
	}
}

void UPredictFootIKAnimInstance::SetFootIKOffset()
{

	USkeletalMeshComponent* Mesh = GetOwningComponent();

	FVector LOffsetTarget = FVector::ZeroVector;
	FVector ROffsetTarget = FVector::ZeroVector;

	FVector CurrentFootLoctionL = CalculateFootLocAtTime(Leader.LFootUpTime, CurrentBlendCurveValue.LeaderUpFootTime, LeftFootValue, false);

	LOffsetTarget = CurrentFootLoctionL - Mesh->GetSocketLocation(Name_Bone_Root) - PredictConfig.FootHeightModify;

	FVector CurrentFootLoctionR = CalculateFootLocAtTime(Leader.RFootUpTime, CurrentBlendCurveValue.LeaderUpFootTime, RightFootValue, false);

	ROffsetTarget = CurrentFootLoctionR - Mesh->GetSocketLocation(Name_Bone_Root) - PredictConfig.FootHeightModify;

	if (IsApplyInterp || InitInpterpTime >= 0.0f)
	{
		InterpSpeed = InitInpterpTime >= 0.0f ? 30.0f : 20.0f;
		AminGraphApplyValues.L_FootIK_Offset = UKismetMathLibrary::VInterpTo(AminGraphApplyValues.L_FootIK_Offset, LOffsetTarget, DeltaTime, InterpSpeed);
		AminGraphApplyValues.R_FootIK_Offset = UKismetMathLibrary::VInterpTo(AminGraphApplyValues.R_FootIK_Offset, ROffsetTarget, DeltaTime, InterpSpeed);
	}
	else {
		AminGraphApplyValues.L_FootIK_Offset = LOffsetTarget;
		AminGraphApplyValues.R_FootIK_Offset = ROffsetTarget;
	}
}

FVector UPredictFootIKAnimInstance::CalculateFootLocAtTime(float Time, float FootUpTime,const FPredictFootValue& FootValue, bool Apply)
{
	if (FootUpTime == 0.0f)
	{
		return FootValue.Foot_End_Location;
	}
	FVector FootStartLoc = FootValue.Foot_Start_Location;
	FVector FootEndLoc = FootValue.Foot_End_Location;
	float TotalDistance = 0.0f;
	if (Apply)
	{
		TotalDistance = TotalDistance = UKismetMathLibrary::Vector_Distance2D(FootStartLoc, FootEndLoc);
	}
	else {
		if (FootValue.Foot_Transition_Points.Num() > 0)
		{
			for (int32 i = 0;i < FootValue.Foot_Transition_Points.Num(); i++)
			{
				if (i == 0)
				{
					TotalDistance += UKismetMathLibrary::Vector_Distance(FootStartLoc, FootValue.Foot_Transition_Points[i]);
					continue;
				}
				TotalDistance += UKismetMathLibrary::Vector_Distance(FootValue.Foot_Transition_Points[i - 1], FootValue.Foot_Transition_Points[i]);
			}

			TotalDistance += UKismetMathLibrary::Vector_Distance(FootEndLoc, FootValue.Foot_Transition_Points[FootValue.Foot_Transition_Points.Num() - 1]);
		}
		else
		{
			TotalDistance = UKismetMathLibrary::Vector_Distance(FootStartLoc, FootEndLoc);
		}
	}

	FVector SubLoc_1 = FootStartLoc;
	float SubTime_1 = FootUpTime;

	for (int32 i = 0;i < FootValue.Foot_Transition_Points.Num(); i++)
	{
		float Dis = 0.0f;
		if (Apply)
		{
			Dis = UKismetMathLibrary::Vector_Distance2D(FootStartLoc, FootValue.Foot_Transition_Points[i]);
		}
		else {
			Dis = UKismetMathLibrary::Vector_Distance(FootStartLoc, FootValue.Foot_Transition_Points[i]);
		}
		
		float SubTime = (1 - (Dis / TotalDistance)) * FootUpTime;

		if (Time >= SubTime)
		{
			return FVector((FootEndLoc.X - FootStartLoc.X) * (1 - Time / FootUpTime) + FootStartLoc.X, (FootEndLoc.Y - FootStartLoc.Y) * (1 - Time / FootUpTime) + FootStartLoc.Y,
				SubLoc_1.Z + (SubTime_1 - Time) / (SubTime_1 - SubTime) * (FootValue.Foot_Transition_Points[i].Z - SubLoc_1.Z));
		}
		SubLoc_1 = FootValue.Foot_Transition_Points[i];
		SubTime_1 = SubTime;
	}
	return FVector((FootEndLoc.X - FootStartLoc.X) * (1 - Time / FootUpTime) + FootStartLoc.X, (FootEndLoc.Y - FootStartLoc.Y) * (1 - Time / FootUpTime) + FootStartLoc.Y,
		SubLoc_1.Z + (SubTime_1 - Time) / (SubTime_1) * (FootEndLoc.Z - SubLoc_1.Z));
}

bool UPredictFootIKAnimInstance::VerticalTraceHit(const FVector& TraceLocation)
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;

	bool bHit = UKismetSystemLibrary::LineTraceMulti(this, TraceLocation + PredictConfig.EnableTrace_Height, TraceLocation, ETraceTypeQuery::TraceTypeQuery9,
		false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

	if (HitResults.Num() > 0 && (HitResults.Last().ImpactPoint.Z - TraceLocation.Z > PredictConfig.Vertical_Trace_Height + PredictConfig.Foot_Box_Trace_Half_Size.X))
	{
		return true;
	}
	return false; 
}


FRotator UPredictFootIKAnimInstance::SetFootIKRotatorOffset(FVector HitNormal)
{
	return FRotator(UKismetMathLibrary::DegAtan2(HitNormal.X, HitNormal.Z) * -1, 0, UKismetMathLibrary::DegAtan2(HitNormal.Y, HitNormal.Z));
}

void UPredictFootIKAnimInstance::SetFootRotatorInterp(FRotator& FootRotatorOffset, const FRotator& FootRotatorTarget)
{
	FootRotatorOffset = UKismetMathLibrary::RInterpTo(FootRotatorOffset, FootRotatorTarget, DeltaTime, 10.0f);
}

void UPredictFootIKAnimInstance::CalculateCurveBlend(FName FootAnimCurveName)
{
	/*
	* 姿态混合度
	*/
	/*初始化 当前混合坐标值*/
	CurrentBlendCurveValue.InitVector();

	/*
	* 姿态 混合度 分解
	*/
	InitAnimBlendRate(WalkCurves);
	InitAnimBlendRate(RunCurves);

	UpdateCurvesBlendRate(WalkCurves, 1 - Grounded.WalkRunBlend, 1);
	UpdateCurvesBlendRate(RunCurves, Grounded.WalkRunBlend, 1);

	UpdateLeaderInfo();

	CalculateApplyBlend(WalkCurves);
	CalculateApplyBlend(RunCurves);

}


void UPredictFootIKAnimInstance::UpdateBlendRate()
{
	for (int32 i = 0; i < BlendNodeArray.Num();i++)
	{
		BlendNodeArray[i].SetWeight(*this, MachineIndex);
	}
}

void UPredictFootIKAnimInstance::UpdateCurvesBlendRate(FCurvesResource& CurvesResource, float PoseBlendRate, float StrideBlendRate)
{
	for (int32 i = 0; i < BlendNodeArray.Num();i++)
	{
		UpdateEachFinalBlend(PoseBlendRate, StrideBlendRate, BlendNodeArray[i].BlendRate, VelocityBlend.F, BlendNodeArray[i].F, CurvesResource);
		UpdateEachFinalBlend(PoseBlendRate, StrideBlendRate, BlendNodeArray[i].BlendRate, VelocityBlend.B, BlendNodeArray[i].B, CurvesResource);
		UpdateEachFinalBlend(PoseBlendRate, StrideBlendRate, BlendNodeArray[i].BlendRate, VelocityBlend.L, BlendNodeArray[i].L, CurvesResource);
		UpdateEachFinalBlend(PoseBlendRate, StrideBlendRate, BlendNodeArray[i].BlendRate, VelocityBlend.R, BlendNodeArray[i].R, CurvesResource);
	}
	
}

void UPredictFootIKAnimInstance::UpdateEachFinalBlend(float PBlendRate, float StrideBlendRate, float NodeBlendRate, float VelocityBlendRate, EPredictHipsDirection& HipsDirection, FCurvesResource& CurvesResource)
{
	FAnimOuterCurves* Curve;
	Curve = CurvesResource.Curves.Find(HipsDirection);
	if (Curve)
	{
		Curve->FinalBlendRate += PBlendRate * NodeBlendRate * VelocityBlendRate * StrideBlendRate;
	}
}

void UPredictFootIKAnimInstance::UpdateLeaderInfo()
{
	float BlendRate = 0.0f;
	for (auto &Item : WalkCurves.Curves)
	{
		if (BlendRate < Item.Value.FinalBlendRate)
		{
			Leader.LeaderAnimType = Item.Key;
			CurrentBlendCurveValue.LeaderAnimLength = Item.Value.AnimationLength;
			CurrentBlendCurveValue.LeaderUpFootTime = Item.Value.Foot_Up_Time;
			BlendRate = Item.Value.FinalBlendRate;
		}
	}

	for (auto &Item : RunCurves.Curves)
	{
		if (BlendRate < Item.Value.FinalBlendRate)
		{
			Leader.LeaderAnimType = Item.Key;
			CurrentBlendCurveValue.LeaderAnimLength = Item.Value.AnimationLength;
			CurrentBlendCurveValue.LeaderUpFootTime = Item.Value.Foot_Up_Time;
			BlendRate = Item.Value.FinalBlendRate;
		}
	}

	CalculateLeaderValue();
}

void UPredictFootIKAnimInstance::CalculateLeaderValue()
{
	float Ratio = CalculateTrsaitionValue();

	float BlendAnimTime = GetCurveValue(Name_CurrentAnimTime);

	if (Ratio < 0.001f)
	{
		Leader.InitValue();
	}
	else
	{
		Leader.CurrentAnimTime = BlendAnimTime / Ratio;

		Leader.LFootUpTime = LUpTime / Ratio;

		Leader.RFootUpTime = RUpTime / Ratio;
	}

}

float UPredictFootIKAnimInstance::CalculateTrsaitionValue()
{
	float Sum = 0.0f;
	float ratesum = 0.0f;
	for (auto &Item : WalkCurves.Curves)
	{
		Sum += Item.Value.AnimationLength * Item.Value.FinalBlendRate;
		ratesum += Item.Value.FinalBlendRate;
	}

	for (auto &Item : RunCurves.Curves)
	{
		Sum += Item.Value.AnimationLength * Item.Value.FinalBlendRate;
		ratesum += Item.Value.FinalBlendRate;
	}

	return Sum / CurrentBlendCurveValue.LeaderAnimLength;
}

void UPredictFootIKAnimInstance::CalculateApplyBlend(FCurvesResource& CurvesResource)
{/*计算坐标混合： 脚部落地点的坐标混合。先计算 单独的 曲线值，再通过单独的曲线值获取曲线资源数据， 最后累加*/
	float LFootUpTime;
	float RFootUpTime;
	for (auto &Item : CurvesResource.Curves)
	{
		float AnimTime = Item.Value.AnimationLength / CurrentBlendCurveValue.LeaderAnimLength * Leader.CurrentAnimTime;
		if (LUpTime > TransitionThreshold)
		{
			LFootUpTime = Item.Value.AnimationLength / CurrentBlendCurveValue.LeaderAnimLength * LUpTime;
			if (LFootUpTime + AnimTime > Item.Value.AnimationLength)
			{
				LFootUpTime = LFootUpTime + AnimTime - Item.Value.AnimationLength;
			}
			else
			{
				LFootUpTime += AnimTime;
			}
			
			CurrentBlendCurveValue.L_Foot_Vector += Item.Value.L_Foot_Vector->GetVectorValue(LFootUpTime) * Item.Value.FinalBlendRate;
			CurrentBlendCurveValue.L_Ball_Vector += Item.Value.L_Ball_Vector->GetVectorValue(LFootUpTime) * Item.Value.FinalBlendRate;
			CurrentBlendCurveValue.L_Foot_Rotator += Item.Value.L_Foot_Rotator->GetVectorValue(LFootUpTime) * Item.Value.FinalBlendRate;
		}
		if (RUpTime > TransitionThreshold)
		{
			RFootUpTime = Item.Value.AnimationLength / CurrentBlendCurveValue.LeaderAnimLength * RUpTime;
			

			if (RFootUpTime + AnimTime > Item.Value.AnimationLength)
			{
				
				RFootUpTime = RFootUpTime + AnimTime - Item.Value.AnimationLength;
			}
			else
			{
				RFootUpTime += AnimTime;
			}
			CurrentBlendCurveValue.R_Foot_Vector += Item.Value.R_Foot_Vector->GetVectorValue(RFootUpTime) * Item.Value.FinalBlendRate;
			CurrentBlendCurveValue.R_Ball_Vector += Item.Value.R_Ball_Vector->GetVectorValue(RFootUpTime) * Item.Value.FinalBlendRate;
			CurrentBlendCurveValue.R_Foot_Rotator += Item.Value.R_Foot_Rotator->GetVectorValue(RFootUpTime) * Item.Value.FinalBlendRate;
		}
		
	}
}

void UPredictFootIKAnimInstance::InitAnimBlendRate(FCurvesResource& CurvesResource)
{
	for (auto& Item : CurvesResource.Curves)
	{
		Item.Value.FinalBlendRate = 0.0f;
	}
}

float UPredictFootIKAnimInstance::CalculateVectorAngle(const FVector& Vector_1, const FVector& Vector_2)
{
	return UKismetMathLibrary::Acos(FVector::DotProduct(Vector_1.GetSafeNormal(), Vector_2.GetSafeNormal()));
}

void UPredictFootIKAnimInstance::AdditionalOffset()
{
	FVector RootLocation = GetOwningComponent()->GetSocketLocation(Name_Bone_Root);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, RootLocation, RootLocation + FVector(0, 0, -10),ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore,EDrawDebugTrace::None, HitResult, true);
	FootOffset = FVector::ZeroVector;
	if (bHit&& HitResult.Component.Get())
	{
		if (UKismetStringLibrary::EqualEqual_StrStr(HitActorName, HitResult.Component.Get()->GetPathName()))
		{
			FootOffset = HitResult.Component.Get()->GetComponentLocation() - HitActorLocation;
		}

		HitActorName = HitResult.Component.Get()->GetPathName();
		HitActorLocation = HitResult.Component.Get()->GetComponentLocation();
	}
}

void UPredictFootIKAnimInstance::DrawTrace_Implementation()
{

}

