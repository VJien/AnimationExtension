// Copyright Epic Games, Inc. All Rights Reserved.


#include "Node/AnimGraphNode_SlopeWarping.h"

#define LOCTEXT_NAMESPACE "MomentumNodes"

UAnimGraphNode_SlopeWarping::UAnimGraphNode_SlopeWarping(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FText UAnimGraphNode_SlopeWarping::GetControllerDescription() const
{
	return LOCTEXT("SlopeWarping", "Slope Warping");
}

FText UAnimGraphNode_SlopeWarping::GetTooltipText() const
{
	return LOCTEXT("SlopeWarpingTooltip", "Adjust Feet IK to Match Floor Normal");
}

FLinearColor UAnimGraphNode_SlopeWarping::GetNodeTitleColor() const
{
	return FLinearColor(1.0f, 0.0f, 0.75f);
}

FText UAnimGraphNode_SlopeWarping::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

#undef LOCTEXT_NAMESPACE
