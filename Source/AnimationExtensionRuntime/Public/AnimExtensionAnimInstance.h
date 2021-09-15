// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimExtensionAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONEXTENSIONRUNTIME_API UAnimExtensionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AnimationExtension)
		void UpdateAim(float DeltaTimeX);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AnimationExtension)
		void UpdateActorLean(float DeltaTimeX);
private:
	void UpdateDistanceMatching(float DeltaTimeX);
	void EvalDistanceMatching(float DeltaTimeX);

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Animation)
		bool bEnableDistanceMatching = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Animation)
		bool IsAccelerating = false;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
		float JogDistanceCurveStartTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
		float JogDistanceCurveStopTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
		FVector DistanceMachingLocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Animation)
		float AimYaw = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Animation)
		float AimPitch = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Animation)
		float YawDelta = 0.0f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimSequence* JogStartAnimSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimSequence* JogStopAnimSequence = nullptr;
private:
	FRotator RotationLastTick;
};

