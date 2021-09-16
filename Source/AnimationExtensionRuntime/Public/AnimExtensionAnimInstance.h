// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimExtensionAnimInstance.generated.h"

class UAnimSequenceBase;



UENUM(BlueprintType)
enum class EOrientDirection : uint8
{
	F,
	R,
	B,
	L
};




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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AnimationExtension)
		void UpdateOrientation(float DeltaTimeX);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure ,Category = AnimationExtension)
		UAnimSequence* GetJogStartAnim();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = AnimationExtension)
		UAnimSequence* GetJogStopAnim();

private:
	void UpdateDistanceMatching(float DeltaTimeX);
	void EvalDistanceMatching(float DeltaTimeX);



public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Animation)
		bool bEnableDistanceMatching = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Animation)
		bool IsAccelerating = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Orientation)
		EOrientDirection OrientDirection;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Orientation)
		float Direction;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Orientation)
		float OrientationAngle;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Orientation)
		float OrientationAlpha;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Orientation)
		float OrientationAngleLerpSpeed = 5.0;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Orientation)
		float OrientationAngleResetSpeed = 0.03;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Orientation)
		float OrientationAlphaLerpSpeed = 0.1;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Orientation)
		float OrientationAlphaResetSpeed = 0.1;


	UPROPERTY(BlueprintReadOnly, Category = DistanceMatching)
		float JogDistanceCurveStartTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = DistanceMatching)
		float JogDistanceCurveStopTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = DistanceMatching)
		FVector DistanceMachingLocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Aim)
		float AimYaw = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Aim)
		float AimPitch = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Lean)
		float YawDelta = 0.0f;





	UPROPERTY(BlueprintReadOnly, Category = DistanceMatching)
		UAnimSequence* CurrentJogStartAnimSequence = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = DistanceMatching)
		UAnimSequence* CurrentJogStopAnimSequence = nullptr;
private:
	FRotator RotationLastTick;
};

