// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimExtensionBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONEXTENSIONRUNTIME_API UAnimExtensionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation Extension")
		static bool PredictStopLocation(FVector& OutStopLocation, const FVector& CurrentLocation, const FVector& Velocity,
			const FVector& Acceleration, float Friction, float BrakingDeceleration, const float TimeStep, const int MaxSimulationIterations /*= 10*/);

	UFUNCTION(BlueprintCallable, Category = "Animation Extension")
		static float GetDistanceCurveTime(UAnimSequence* Sequence, float Distance, FName DistanceCurveName = "DistanceCurve");



};
