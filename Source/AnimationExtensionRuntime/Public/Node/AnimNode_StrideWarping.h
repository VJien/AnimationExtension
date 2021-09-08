// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SpringInterpolator.h"
#include "AnimNode_StrideWarping.generated.h"

struct FInputScaleBiasClamp;

UENUM(BlueprintType)
enum class EStrideWarpingAxisMode : uint8
{
	IKFootRootLocalX,
	IKFootRootLocalY,
	IKFootRootLocalZ,
	WorldSpaceVectorInput,
	ComponentSpaceVectorInput,
	ActorSpaceVectorInput,
};

/** Per foot definitions */
USTRUCT()
struct FStrideWarpingFootDefinition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference IKFootBone;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference FKFootBone;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 NumBonesInLimb;

	FStrideWarpingFootDefinition()
		: NumBonesInLimb(2)
	{}
};

/** Runtime foot data after validation, we guarantee these bones to exist */
USTRUCT()
struct FStrideWarpingFootData
{
	GENERATED_USTRUCT_BODY()

	FCompactPoseBoneIndex IKFootBoneIndex;
	FCompactPoseBoneIndex FKFootBoneIndex;
	FCompactPoseBoneIndex HipBoneIndex;
	FTransform IKBoneTransform;
	//��
	FTransform FKBoneTransform;
	FStrideWarpingFootData()
		: IKFootBoneIndex(INDEX_NONE)
		, FKFootBoneIndex(INDEX_NONE)
		, HipBoneIndex(INDEX_NONE)
	{}
};

USTRUCT()
struct ANIMATIONEXTENSIONRUNTIME_API FAnimNode_StrideWarping : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	FAnimNode_StrideWarping();

	FAnimInstanceProxy* MyAnimInstanceProxy;

	/** IKFoot Root Bone. **/
	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference IKFootRootBone;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<FStrideWarpingFootDefinition> FeetDefinitions;

	UPROPERTY(Transient)
	TArray<FStrideWarpingFootData> FeetData;

	/** Pelvis Bone. **/
	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference PelvisBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EStrideWarpingAxisMode StrideWarpingAxisMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EStrideWarpingAxisMode FloorNormalAxisMode;

	/** Direction vector for adjusting hips up and down. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EStrideWarpingAxisMode GravityDirAxisMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (PinShownByDefault))
	float StrideScaling;

	// Additional scaling, offsetting and clamping of PlayRate input.
	// Performed after PlayRateBasis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FInputScaleBiasClamp StrideScalingScaleBiasClamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (PinHiddenByDefault))
	FVector ManualStrideWarpingDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (PinHiddenByDefault))
	FVector ManualFloorNormalInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (PinHiddenByDefault))
	FVector ManualGravityDirInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float PelvisPostAdjustmentAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 PelvisAdjustmentMaxIter;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVectorRK4SpringInterpolator PelvisAdjustmentInterp;

	UPROPERTY(EditAnywhere, Category = "Settings")
	uint32 bAdjustThighBonesRotation : 1;

	UPROPERTY(EditAnywhere, Category = "Settings")
	uint32 bClampIKUsingFKLeg : 1;

	UPROPERTY(EditAnywhere, Category = "Settings")
	uint32 bOrientStrideWarpingAxisBasedOnFloorNormal : 1;

	UPROPERTY(Transient)
	float CachedDeltaTime;

	FVector GetAxisModeValue(const EStrideWarpingAxisMode& AxisMode, const FTransform& IKFootRootCSTransform, const FVector& UserSuppliedVector) const;
	FCompactPoseBoneIndex FindHipBoneIndex(const FCompactPoseBoneIndex& InFootBoneIndex, const int32& NumBonesInLimb, const FBoneContainer& RequiredBones) const;

public:
	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface
};
