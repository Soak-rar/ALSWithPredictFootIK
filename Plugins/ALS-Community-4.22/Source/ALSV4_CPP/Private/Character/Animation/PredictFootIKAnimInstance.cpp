// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/PredictFootIKAnimInstance.h"


static const FName Name_L_Foot_UptoDown(TEXT("L_UptoDown"));
static const FName Name_R_Foot_UptoDown(TEXT("R_UptoDown"));
static const FName Name_CurrentAnimTime(TEXT("CurrentAnimTime"));
static const FName Name_Bone_Root(TEXT("Root"));
static const FName Name_Bone_Pelvis(TEXT("Pelvis"));
static const FName Name_IKBone_FootL(TEXT("VB foot_target_l"));
static const FName Name_IKBone_FootR(TEXT("VB foot_target_r"));
static const FName Name_Bone_FootL(TEXT("Foot_L"));
static const FName Name_Bone_FootR(TEXT("Foot_R"));


void UPredictFootIKAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPredictFootIKAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	InitBlendNode();
	PredictFootIKValues.Foot_L_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootL);
	PredictFootIKValues.Foot_R_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootR);
}

void UPredictFootIKAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UPredictFootIKAnimInstance::UpdatePredictFootIK(float DeltaSeconds)
{
	DeltaTime = DeltaSeconds;
	if (Character)
	{
		Speed = Character->GetVelocity();
	}
	UpdateBlendRate();
	LUpTime = GetCurveValue(Name_L_Foot_UptoDown);
	RUpTime = GetCurveValue(Name_R_Foot_UptoDown);
	//UKismetSystemLibrary::PrintString(this, FString::FromInt(IsApplyFootIK));
	if (MovementState == EALSMovementState::Grounded)
	{

		
		if (Speed.Size() == 0|| IsApplyFootIK)
		{
			/*应用脚部IK*/
			AminGraphApplyValues.L_FootIK_Offset = FootIKValues.FootOffset_L_Location.Z;
			AminGraphApplyValues.R_FootIK_Offset = FootIKValues.FootOffset_R_Location.Z;
			AminGraphApplyValues.PelvisOffset = FootIKValues.PelvisOffset.Z;


			PredictFootIKValues.Foot_L_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootL);
			PredictFootIKValues.Foot_R_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootR);
			/*	AminGraphApplyValues.InitValue();*/


			UpdatePredictIKData(LUpTime, PredictFootIKValues.L_Root_Start_Location, PredictFootIKValues.Foot_L_Start_Location,
				PredictFootIKValues.Foot_L_Transition_Points, PredictFootIKValues.Foot_L_End_Location);
			UpdatePredictIKData(RUpTime, PredictFootIKValues.R_Root_Start_Location, PredictFootIKValues.Foot_R_Start_Location,
				PredictFootIKValues.Foot_R_Transition_Points, PredictFootIKValues.Foot_R_End_Location);

		}
		else
		{
			/*应用预测IK*/
			CalculateCurveBlend(Name_L_Foot_UptoDown);

			UpdatePredictIKData(LUpTime, PredictFootIKValues.L_Root_Start_Location, PredictFootIKValues.Foot_L_Start_Location,
				PredictFootIKValues.Foot_L_Transition_Points, PredictFootIKValues.Foot_L_End_Location);
			UpdatePredictIKData(RUpTime, PredictFootIKValues.R_Root_Start_Location, PredictFootIKValues.Foot_R_Start_Location,
				PredictFootIKValues.Foot_R_Transition_Points, PredictFootIKValues.Foot_R_End_Location);
			if (LUpTime > 0.01)
			{
				SetPredictFootOffsetWithRotator(Name_L_Foot_UptoDown, Speed, PredictFootIKValues.L_Root_Start_Location, PredictFootIKValues.L_Root_End_Location,
					PredictFootIKValues.Foot_L_Start_Location, PredictFootIKValues.Foot_L_End_Location, PredictFootIKValues.Foot_L_Transition_Points, CurrentBlendCurveValue.L_Foot_Vector, CurrentBlendCurveValue.L_Foot_Rotator, CurrentBlendCurveValue.L_Ball_Vector, PredictFootIKValues.L_FootRotator_Target);
				SetFootRotatorInterp(AminGraphApplyValues.L_FootRotator_Offset, PredictFootIKValues.L_FootRotator_Target);
			}
			else if (RUpTime > 0.01)
			{

				SetPredictFootOffsetWithRotator(Name_R_Foot_UptoDown, Speed, PredictFootIKValues.R_Root_Start_Location, PredictFootIKValues.R_Root_End_Location,
					PredictFootIKValues.Foot_R_Start_Location, PredictFootIKValues.Foot_R_End_Location, PredictFootIKValues.Foot_R_Transition_Points, CurrentBlendCurveValue.R_Foot_Vector, CurrentBlendCurveValue.R_Foot_Rotator, CurrentBlendCurveValue.R_Ball_Vector, PredictFootIKValues.R_FootRotator_Target);
				SetFootRotatorInterp(AminGraphApplyValues.R_FootRotator_Offset, PredictFootIKValues.R_FootRotator_Target);
			}

			SetFootIKOffset(PredictFootIKValues, Name_IKBone_FootL, Name_IKBone_FootR, AminGraphApplyValues);

			if (LUpTime > 0.01)
			{
				SetPelvisOffset(PredictFootIKValues.Foot_L_Start_Location, PredictFootIKValues.Foot_L_End_Location,
					PredictFootIKValues.Foot_R_End_Location, Name_L_Foot_UptoDown, PredictFootIKValues.Foot_L_Transition_Points,
					AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.Foot_Up_Time);
			}
			else if (RUpTime > 0.01)
			{
				SetPelvisOffset(PredictFootIKValues.Foot_R_Start_Location, PredictFootIKValues.Foot_R_End_Location,
					PredictFootIKValues.Foot_L_End_Location, Name_R_Foot_UptoDown, PredictFootIKValues.Foot_R_Transition_Points,
					AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.Foot_Up_Time);
			}
			//if ((LUpTime>0 && LUpTime < 0.05) || (RUpTime > 0 && RUpTime < 0.05))
			//{
			//	UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(FootIKValues.PelvisOffset.Z).Append("    ").Append(FString::SanitizeFloat(AminGraphApplyValues.PelvisOffset)));
			//}
			
		}

		//if (TransitionTime > 0)
		//{
		//	TransitionTime -= DeltaSeconds;
		//	UKismetMathLibrary::FInterpTo_Constant(LockedPIK, AminGraphApplyValues.PelvisOffset, DeltaSeconds, 50.0f);
		//	UKismetMathLibrary::FInterpTo_Constant(LockedLIK, AminGraphApplyValues.L_FootIK_Offset, DeltaSeconds, 50.0f);
		//	UKismetMathLibrary::FInterpTo_Constant(LockedRIK, AminGraphApplyValues.R_FootIK_Offset, DeltaSeconds, 50.0f);
		//	UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(TransitionTime));
		//	AminGraphApplyValues.PelvisOffset = LockedPIK;
		//	AminGraphApplyValues.L_FootIK_Offset = LockedLIK;
		//	AminGraphApplyValues.R_FootIK_Offset = LockedRIK;
		//}
	

		
	}
	//UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(TransitionTime));
	//if (MovementState == EALSMovementState::Grounded && Speed.Size() > 0)
	//{
	//	CalculateCurveBlend(Name_L_Foot_UptoDown);

	//	UpdatePredictIKData(LUpTime, PredictFootIKValues.L_Root_Start_Location, PredictFootIKValues.Foot_L_Start_Location,
	//		PredictFootIKValues.Foot_L_Transition_Points, PredictFootIKValues.Foot_L_End_Location);
	//	UpdatePredictIKData(RUpTime,  PredictFootIKValues.R_Root_Start_Location, PredictFootIKValues.Foot_R_Start_Location,
	//		PredictFootIKValues.Foot_R_Transition_Points, PredictFootIKValues.Foot_R_End_Location);

	//	if (LUpTime > 0.01)
	//	{
	//		SetPredictFootOffsetWithRotator(Name_L_Foot_UptoDown, Speed, PredictFootIKValues.L_Root_Start_Location, PredictFootIKValues.L_Root_End_Location,
	//			PredictFootIKValues.Foot_L_Start_Location, PredictFootIKValues.Foot_L_End_Location, PredictFootIKValues.Foot_L_Transition_Points, CurrentBlendCurveValue.L_Foot_Vector, CurrentBlendCurveValue.L_Foot_Rotator, CurrentBlendCurveValue.L_Ball_Vector, PredictFootIKValues.L_FootRotator_Target);
	//		SetFootRotatorInterp(AminGraphApplyValues.L_FootRotator_Offset, PredictFootIKValues.L_FootRotator_Target);
	//	}
	//	else if (RUpTime > 0.01)
	//	{

	//		SetPredictFootOffsetWithRotator(Name_R_Foot_UptoDown, Speed, PredictFootIKValues.R_Root_Start_Location, PredictFootIKValues.R_Root_End_Location,
	//			PredictFootIKValues.Foot_R_Start_Location, PredictFootIKValues.Foot_R_End_Location, PredictFootIKValues.Foot_R_Transition_Points, CurrentBlendCurveValue.R_Foot_Vector, CurrentBlendCurveValue.R_Foot_Rotator, CurrentBlendCurveValue.R_Ball_Vector, PredictFootIKValues.R_FootRotator_Target);
	//		SetFootRotatorInterp(AminGraphApplyValues.R_FootRotator_Offset, PredictFootIKValues.R_FootRotator_Target);
	//	}

	//	SetFootIKOffset(PredictFootIKValues, Name_IKBone_FootL, Name_IKBone_FootR, AminGraphApplyValues);

	//	if (LUpTime > 0.01)
	//	{
	//		SetPelvisOffset(PredictFootIKValues.Foot_L_Start_Location, PredictFootIKValues.Foot_L_End_Location,
	//			PredictFootIKValues.Foot_R_End_Location, Name_L_Foot_UptoDown, PredictFootIKValues.Foot_L_Transition_Points,
	//			AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.Foot_Up_Time);
	//	}
	//	else if (RUpTime > 0.01)
	//	{
	//		SetPelvisOffset(PredictFootIKValues.Foot_R_Start_Location, PredictFootIKValues.Foot_R_End_Location,
	//			PredictFootIKValues.Foot_L_End_Location, Name_R_Foot_UptoDown, PredictFootIKValues.Foot_R_Transition_Points,
	//			AminGraphApplyValues.PelvisOffset, CurrentBlendCurveValue.Foot_Up_Time);
	//	}
	//	//UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(Speed.Size()));
	//	

	//	
	//}
	//else
	//{
	//	PredictFootIKValues.Foot_L_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootL);
	//	PredictFootIKValues.Foot_R_End_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_FootR);
	//	AminGraphApplyValues.InitValue();
	//}
	DrawTrace();
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

void UPredictFootIKAnimInstance::SetPredictFootOffsetWithRotator(FName FootAnimCurveName, FVector Velocity, FVector& RootStartLocation, FVector& RootEndLocation,
	FVector& StartLocation, FVector& EndLocation, TArray<FVector>& TransitionPoints, FVector FootCurveVector, FVector FootCurveRotator, FVector BallCurveVector,
	FRotator& FootRotatorTarget)
{
	// 计算落脚位置
	FTransform CurrentRootTransform = GetOwningComponent()->GetSocketTransform(Name_Bone_Root);
	FTransform PredictRootTransform = FTransform(CurrentRootTransform.GetRotation(), (GetCurveValue(FootAnimCurveName) * Velocity + CurrentRootTransform.GetLocation()), CurrentRootTransform.GetScale3D());
	float CurveTime = GetCurveValue(Name_CurrentAnimTime) + GetCurveValue(FootAnimCurveName);
	if (CurveTime >= CurrentBlendCurveValue.AnimationLength)
	{
		CurveTime -= CurrentBlendCurveValue.AnimationLength;
	}

	FVector FootEndLocation = PredictRootTransform.TransformPosition(FootCurveVector);

	/**/
	FVector PredictBallLocation = PredictRootTransform.TransformPosition(BallCurveVector);

	const FVector TraceCenter = (FootEndLocation * 0.5 + PredictBallLocation * 0.5);

	const FVector FootRotatorVector = FootCurveRotator;

	FQuat FootEndRotator = PredictRootTransform.TransformRotation(FQuat(FRotator::MakeFromEuler(FootRotatorVector)));

	/**/

	UWorld* World = GetWorld();
	check(World);

	TArray<AActor*> ActorsToIgnore;

	/**/
	const FVector TraceStart = TraceCenter + FVector(0.0, 0.0, PredictConfig.Vertical_Trace_Height);
	const FVector TraceEnd = TraceCenter - FVector(0.0, 0.0, PredictConfig.Vertical_Trace_Height);

	/**/
	FHitResult HitResult;

	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this, TraceStart, TraceEnd, PredictConfig.Foot_Box_Trace_Half_Size, FRotator(90, FootEndRotator.Rotator().Yaw, 90),
		ETraceTypeQuery::TraceTypeQuery_MAX, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true,
		FLinearColor::Red, FLinearColor::Green, CurrentBlendCurveValue.AnimationLength);

	/*不可达： 初始碰撞， 高点射线 检测*/

	bool bVerticalHit = VerticalTraceHit(FootEndLocation);

	if (bHit && (!HitResult.bStartPenetrating) && (!bVerticalHit))
	{
		EndLocation = HitResult.Location + (FootEndLocation - TraceCenter);


		FootRotatorTarget = SetFootIKRotatorOffset(HitResult.ImpactNormal);

		/*修正脚在斜面的 Z位置 待优化*/
		if (HitResult.ImpactNormal.Z < 1)
		{
			//EndLocation -= FVector(0, 0, 5);
		}
		EndLocation -= FVector(0, 0, 5);
		UKismetSystemLibrary::DrawDebugPoint(this, EndLocation, 4.0f, FLinearColor::Blue);
	}

	else
	{
		EndLocation = FootEndLocation;
	}

	TraceTransitionPoints(StartLocation, EndLocation, TransitionPoints);
}

void UPredictFootIKAnimInstance::UpdatePredictIKData(float FootUpTime, FVector& Root_Start_Location, FVector& Foot_Start_Location, TArray<FVector>& TransitionPoints, FVector Foot_End_Location)
{
	// 根据脚步曲线更新角色起点位置，曲线值 =0 更新，>0 不更新
	if (FootUpTime < 0.01)
	{
		if (TransitionPoints.Num() != 0)
		{
			TransitionPoints.Empty();
		}
		Root_Start_Location = GetOwningComponent()->GetSocketLocation(Name_Bone_Root);
		FTransform RootTransform = GetOwningComponent()->GetSocketTransform(Name_Bone_Root);

		//Foot_Start_Location = GetOwningComponent()->GetSocketLocation(FootStartName);
		Foot_Start_Location = Foot_End_Location;
	}
}

bool UPredictFootIKAnimInstance::TraceTransitionPoints(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints)
{
	FVector SubVector = FootEndLocation - FootStartLocation;
	float LineLength = SubVector.Size();
	TArray<AActor*> ActorsToIgnore;
	TransitionPoints.Empty();

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
			bHit = UKismetSystemLibrary::BoxTraceSingle(this, TraceStartLoc, TraceEndLoc, PredictConfig.Foot_Box_Trace_Half_Size, Character->GetActorRotation(),
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
				TransitionPoints.Add(HitResult.Location + FVector(0, 0, 8));
			}
		}
	}
	// 过滤列表：
	TransitionPointsFilter(FootStartLocation, FootEndLocation, TransitionPoints);
	//TransitionPoints.Add(FootEndLocation);
	return true;
}

void UPredictFootIKAnimInstance::TransitionPointsFilter(const FVector& FootStartLocation, const FVector& FootEndLocation, TArray<FVector>& TransitionPoints)
{// 过渡点与起始终点做成的路线应该为 上凸曲线，删除造成下凸的过渡点
	TArray<int32> Temporary;
	FVector Start;
	FVector End;

	for (int32 i = 0; i < TransitionPoints.Num(); i++)
	{
		if (i == 0)
		{
			Start = FootStartLocation;
		}
		else
		{
			Start = TransitionPoints[i - 1];
		}

		if (i == TransitionPoints.Num() - 1)
		{
			End = FootEndLocation;
		}
		else
		{
			End = TransitionPoints[i + 1];
		}

		float Expect_Z = (End.Z - Start.Z) * UKismetMathLibrary::Vector_Distance2D(Start, TransitionPoints[i]) / UKismetMathLibrary::Vector_Distance2D(Start, End) + Start.Z;

		if (TransitionPoints[i].Z <= Expect_Z)
		{
			Temporary.Add(i);
		}

	}

	for (int32 j = Temporary.Num() - 1; j >= 0; j--)
	{
		TransitionPoints.RemoveAt(j);
	}
}

void UPredictFootIKAnimInstance::SetPelvisOffset(FVector TraceStartLoc, FVector FootEndLoc, FVector FootLandedLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float& PelvisOffset, float TraceTime)
{
	USkeletalMeshComponent* Mesh = GetOwningComponent();

	FVector CurrentFootLoction = CalculateFootLocAtTime(TraceStartLoc, FootEndLoc, FootAnimCurveName, TransitionPoints, TraceTime);

	if (CurrentFootLoction.Z >= FootLandedLoc.Z)
	{

		PelvisOffset = FootLandedLoc.Z - Mesh->GetSocketLocation(Name_Bone_Root).Z - CurrentBlendCurveValue.Foot_Root_Landed_Height;
	}
	else
	{
		PelvisOffset = CurrentFootLoction.Z - Mesh->GetSocketLocation(Name_Bone_Root).Z - CurrentBlendCurveValue.Foot_Root_Landed_Height;
	}
}

void UPredictFootIKAnimInstance::SetFootIKOffset(FPredictFootIK& IKValues, FName IKFoot_L, FName IKFoot_R, FAminGraphApplyValue& AnimValues)
{

	USkeletalMeshComponent* Mesh = GetOwningComponent();

	//FVector CurrentFootLoctionL = CalculateFootLocAtTime(IKValues.Foot_L_Start_Location, IKValues.Foot_L_End_Location, Name_L_Foot_UptoDown, IKValues.Foot_L_Transition_Points, CurrentBlendCurveValue.Foot_Up_Time);
	FVector CurrentFootLoctionL = CalculateFootLocAtTime(IKValues.Foot_L_Start_Location, IKValues.Foot_L_End_Location, Name_L_Foot_UptoDown, IKValues.Foot_L_Transition_Points, CurrentBlendCurveValue.Foot_Up_Time);
	AnimValues.L_FootIK_Offset = CurrentFootLoctionL.Z - Mesh->GetSocketLocation(Name_Bone_Root).Z - PredictConfig.FootHeightModify;


	FVector CurrentFootLoctionR = CalculateFootLocAtTime(IKValues.Foot_R_Start_Location, IKValues.Foot_R_End_Location, Name_R_Foot_UptoDown, IKValues.Foot_R_Transition_Points, CurrentBlendCurveValue.Foot_Up_Time);

	AnimValues.R_FootIK_Offset = CurrentFootLoctionR.Z - Mesh->GetSocketLocation(Name_Bone_Root).Z - PredictConfig.FootHeightModify;

}

FVector UPredictFootIKAnimInstance::CalculateFootLocAtTime(FVector FootStartLoc, FVector FootEndLoc, FName FootAnimCurveName, TArray<FVector>& TransitionPoints, float FootUpTime)
{

	float Time = GetCurveValue(FootAnimCurveName);
	float TotalDistance = UKismetMathLibrary::Vector_Distance2D(FootStartLoc, FootEndLoc);
	FVector SubLoc_1 = FootStartLoc;
	float SubTime_1 = FootUpTime;

	for (int32 i = 0;i < TransitionPoints.Num(); i++)
	{
		float Dis = UKismetMathLibrary::Vector_Distance2D(FootStartLoc, TransitionPoints[i]);
		float SubTime = (1 - (Dis / TotalDistance)) * FootUpTime;

		if (Time >= SubTime)
		{
			return FVector((FootEndLoc.X - FootStartLoc.X) * (1 - Time / FootUpTime) + FootStartLoc.X, (FootEndLoc.Y - FootStartLoc.Y) * (1 - Time / FootUpTime) + FootStartLoc.Y,
				SubLoc_1.Z + (SubTime_1 - Time) / (SubTime_1 - SubTime) * (TransitionPoints[i].Z - SubLoc_1.Z));
		}
		SubLoc_1 = TransitionPoints[i];
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

	if (HitResults.Num() > 0 && (HitResults.Last().ImpactPoint.Z - TraceLocation.Z > PredictConfig.Vertical_Trace_Height + 15.0f))
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
	* 姿态混合度 ，待优化
	*/
	CurrentBlendCurveValue.Foot_Root_Landed_Height = WalkCurves.Foot_Root_Landed_Height;
	CurrentBlendCurveValue.InitVector();

	/*姿态混合*/

	/*方向混合*/

	for (int32 j = 0; j < BlendNodeArray.Num();j++)
	{
		//UKismetSystemLibrary::PrintString(this, BlendNodeArray[i].StateName.ToString().Append(FString::SanitizeFloat(BlendNodeArray[i].BlendRate)));
		if (BlendNodeArray[j].BlendRate == 1.0f)
		{
			//for (int32 i = 0; i < BlendNodeArray.Num();i++)
			//{
			//	//UKismetSystemLibrary::PrintString(this, BlendNodeArray[i].StateName.ToString().Append(FString::SanitizeFloat(BlendNodeArray[i].BlendRate)));
			//	NodeBlend(PoseBlendRate, BlendNodeArray[i]);
			//}
			//break;
			NodeBlend(BlendNodeArray[j]);
			break;
		}
	}

	//for (int32 i = 0; i < BlendNodeArray.Num();i++)
	//{
	//	//UKismetSystemLibrary::PrintString(this, BlendNodeArray[i].StateName.ToString().Append(FString::SanitizeFloat(BlendNodeArray[i].BlendRate)));
	//		NodeBlend(PoseBlendRate, BlendNodeArray[i]);
	//}
}

void UPredictFootIKAnimInstance::NodeBlend(const FBlendNode& BlendNode)
{
	
	HipsBlend(BlendNode.BlendRate, VelocityBlend.F, BlendNode.F);

	HipsBlend(BlendNode.BlendRate, VelocityBlend.B, BlendNode.B);

	HipsBlend(BlendNode.BlendRate, VelocityBlend.L, BlendNode.L);

	HipsBlend(BlendNode.BlendRate, VelocityBlend.R, BlendNode.R);

}

void UPredictFootIKAnimInstance::HipsBlend(float NodeBlendRate, float VelocityBlendRate, const EPredictHipsDirection& HipsDirection)
{
	auto WalkCurve = WalkCurves.Curves.Find(HipsDirection);
	auto RunCurve = RunCurves.Curves.Find(HipsDirection);

	switch (CurrentState)
	{
	case EBaseState::None:
		break;
	case EBaseState::Walk:
		if (WalkCurve)
		{
			FinalBlend(1.0f, NodeBlendRate, VelocityBlendRate, *WalkCurve);
		}
		break;
	case EBaseState::Run:
		if (RunCurve)
		{
			FinalBlend(1.0f, NodeBlendRate, VelocityBlendRate, *RunCurve);
		}
		break;
	case EBaseState::Crouch:
		break;
	default:
		break;
	}
	
}

void UPredictFootIKAnimInstance::FinalBlend(float PBlendRate, float NodeBlendRate, float VelocityBlendRate, const FAnimOuterCurves& OuterCurves)
{
	CurrentBlendCurveValue.AnimationLength = OuterCurves.AnimationLength;
	CurrentBlendCurveValue.Foot_Up_Time = OuterCurves.Foot_Up_Time;

	float AnimTime = GetCurveValue(Name_CurrentAnimTime);
	float LPredictTime = AnimTime + LUpTime;
	float FinalBlendRate = NodeBlendRate * VelocityBlendRate;
	if (LUpTime > 0.01)
	{
		if (LPredictTime >= CurrentBlendCurveValue.AnimationLength)
		{
			LPredictTime -= CurrentBlendCurveValue.AnimationLength;
		}
		CurrentBlendCurveValue.L_Foot_Vector += OuterCurves.L_Foot_Vector->GetVectorValue(LPredictTime)* FinalBlendRate;
		CurrentBlendCurveValue.L_Ball_Vector += OuterCurves.L_Ball_Vector->GetVectorValue(LPredictTime)* FinalBlendRate;
		CurrentBlendCurveValue.L_Foot_Rotator += OuterCurves.L_Foot_Rotator->GetVectorValue(LPredictTime)* FinalBlendRate;
	}

	float RPredictTime = AnimTime + RUpTime;
	if (RUpTime > 0.01)
	{
		if (RPredictTime >= CurrentBlendCurveValue.AnimationLength)
		{
			RPredictTime -= CurrentBlendCurveValue.AnimationLength;
		}
		CurrentBlendCurveValue.R_Foot_Vector += OuterCurves.R_Foot_Vector->GetVectorValue(RPredictTime)* FinalBlendRate;
		CurrentBlendCurveValue.R_Ball_Vector += OuterCurves.R_Ball_Vector->GetVectorValue(RPredictTime)* FinalBlendRate;
		CurrentBlendCurveValue.R_Foot_Rotator += OuterCurves.R_Foot_Rotator->GetVectorValue(RPredictTime)* FinalBlendRate;
	}
}

void UPredictFootIKAnimInstance::UpdateBlendRate()
{
	bool flag = true;
	for (int32 i = 0; i < BlendNodeArray.Num();i++)
	{
		BlendNodeArray[i].SetWeight(*this, MachineIndex);
		if (BlendNodeArray[i].BlendRate == 1.0f)
		{
			UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(LUpTime).Append("    ").Append(FString::SanitizeFloat(RUpTime)));
			flag = false;
		}
		if (BlendNodeArray[i].IsTransition)
		{
			TransitionTime = PredictConfig.MaxTransTime;
			LockedPIK = AminGraphApplyValues.PelvisOffset;
		}
	
	}

	IsApplyFootIK = flag;
}

void UPredictFootIKAnimInstance::DrawTrace_Implementation()
{

}