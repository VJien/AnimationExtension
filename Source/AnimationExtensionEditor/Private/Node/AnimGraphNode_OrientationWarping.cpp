// Copyright Epic Games, Inc. All Rights Reserved.


#include "Node/AnimGraphNode_OrientationWarping.h"

/////////////////////////////////////////////////////
// UAnimGraphNode_OrientationWarping

#define LOCTEXT_NAMESPACE "MomentumNodes"

UAnimGraphNode_OrientationWarping::UAnimGraphNode_OrientationWarping(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_OrientationWarping::GetControllerDescription() const
{
	return LOCTEXT("OrientationWarping", "Orientation Warping");
}

FText UAnimGraphNode_OrientationWarping::GetTooltipText() const
{
	return LOCTEXT("OrientationWarpingTooltip", "Orients RootBone to match locomotion direction, and counter rotates spine.");
}

FLinearColor UAnimGraphNode_OrientationWarping::GetNodeTitleColor() const
{
	return FLinearColor(1.0f, 0.0f, 0.75f);
}

FText UAnimGraphNode_OrientationWarping::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

#undef LOCTEXT_NAMESPACE
