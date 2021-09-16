// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimExtensionAnimInstance.h"
#include "AnimExtensionBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstanceProxy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


#if ENABLE_ANIM_DEBUG
TAutoConsoleVariable<int32> CVarDistanceMatchingDebug(TEXT("a.DistanceMatching.Debug"), 0, TEXT("Turn on debug for DistanceMatching"));
#endif

TAutoConsoleVariable<int32> CVarDistanceMatchinEnable(TEXT("a.DistanceMatching.Enable"), 1, TEXT("Toggle DistanceMatching"));

DECLARE_CYCLE_STAT(TEXT("DistanceMatching Eval"), STAT_DistanceMatching_Eval, STATGROUP_Anim);



PRAGMA_DISABLE_OPTIMIZATION


void UAnimExtensionAnimInstance::NativeInitializeAnimation()
{
	//Very Important Line
	Super::NativeInitializeAnimation();
}

void UAnimExtensionAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	//Very Important Line
	Super::NativeUpdateAnimation(DeltaTimeX);
	if (bEnableDistanceMatching)
	{
		UpdateDistanceMatching(DeltaTimeX);
		EvalDistanceMatching(DeltaTimeX);
	}
	UpdateOrientation(DeltaTimeX);
	
}

void UAnimExtensionAnimInstance::UpdateAim_Implementation(float DeltaTimeX)
{
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!ensure(Character))
		return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!ensure(Mesh))
		return;

	FRotator BaseAimRotation = Pawn->GetBaseAimRotation(); // Camera Rotation
	FRotator MeshRotation = Mesh->GetComponentRotation() - Character->GetBaseRotationOffsetRotator();
	FRotator Delta = BaseAimRotation - MeshRotation;
	Delta.Normalize();
	AimYaw = Delta.Yaw;
	AimPitch = Delta.Pitch;
}



void UAnimExtensionAnimInstance::UpdateActorLean_Implementation(float DeltaTimeX)
{
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	FRotator ActorRotation = Pawn->GetActorRotation();
	float Delta = FMath::FindDeltaAngleDegrees(ActorRotation.Yaw, RotationLastTick.Yaw);
	YawDelta = FMath::FInterpTo(YawDelta, Delta / DeltaTimeX, DeltaTimeX, 6);
	//InverseYawDelta = -YawDelta;

	RotationLastTick = ActorRotation;
}

void UAnimExtensionAnimInstance::UpdateOrientation_Implementation(float DeltaTimeX)
{

	auto updatetDirection = [this]()
	{
		float NorDirection = UKismetMathLibrary::ClampAxis(Direction);
		int32 idx = UKismetMathLibrary::Round(NorDirection / 90.0f) % 4;
		OrientDirection = (EOrientDirection)idx;
		CurrentJogStartAnimSequence = GetJogStartAnim();
		CurrentJogStopAnimSequence = GetJogStopAnim();
	};

	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;
	Direction = CalculateDirection(Pawn->GetVelocity(), Pawn->GetActorRotation());
	float NorDirection = UKismetMathLibrary::ClampAxis(Direction);
	if (IsAccelerating)
	{
		if (ACharacter* charc = Cast<ACharacter>(Pawn))
		{
			if (!charc->GetCharacterMovement()->bOrientRotationToMovement)
			{
				switch (OrientDirection)
				{
				case EOrientDirection::F:
				{
					if (!UKismetMathLibrary::InRange_FloatFloat(Direction, -60.f, 60.f, true, true))
					{
						OrientationAlpha = 0.f;
						updatetDirection();
					}
				}
					break;
				case EOrientDirection::R:
				{
					if (!UKismetMathLibrary::InRange_FloatFloat(Direction, 30.f, 150.f, false, false))
					{
						OrientationAlpha = 0.f;
						updatetDirection();
					}
				}
					break;
				case EOrientDirection::B:
					if (!UKismetMathLibrary::InRange_FloatFloat(NorDirection, 120.f, 240.f, true, true))
					{
						OrientationAlpha = 0.f;
						updatetDirection();
					}
					break;
				case EOrientDirection::L:
					if (!UKismetMathLibrary::InRange_FloatFloat(Direction, -150.f, -30.f, false, false))
					{
						OrientationAlpha = 0.f;
						updatetDirection();
					}
					break;
				default:
					break;
				}
			}

			int32 idx = (int32)OrientDirection;
			float target  = UKismetMathLibrary::NormalizeAxis(Direction - idx * 90.0f);
			OrientationAlpha = UKismetMathLibrary::FInterpTo(OrientationAlpha, 1.0f, DeltaTimeX, OrientationAlphaLerpSpeed);
			OrientationAngle = UKismetMathLibrary::FInterpTo(OrientationAngle, target, DeltaTimeX, OrientationAngleLerpSpeed);
		}
	}
	else
	{
		OrientationAlpha = UKismetMathLibrary::FInterpTo(OrientationAlpha, 0.f, DeltaTimeX,  OrientationAlphaResetSpeed);
		OrientationAngle = UKismetMathLibrary::FInterpTo(OrientationAngle, 0.f, DeltaTimeX, OrientationAngleResetSpeed);
	}
}

UAnimSequence* UAnimExtensionAnimInstance::GetJogStartAnim_Implementation()
{
	return CurrentJogStartAnimSequence;
}

UAnimSequence* UAnimExtensionAnimInstance::GetJogStopAnim_Implementation()
{
	return CurrentJogStopAnimSequence;
}

void UAnimExtensionAnimInstance::UpdateDistanceMatching(float DeltaTimeX)
{
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!ensure(Character))
		return;

	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	if (!ensure(CharacterMovement))
		return;

	FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();
	bool IsAcceleratingNow = FVector::DistSquaredXY(CurrentAcceleration, FVector::ZeroVector) > 0;
	if (IsAcceleratingNow == IsAccelerating)
		return;

	IsAccelerating = IsAcceleratingNow;

	if (IsAccelerating)
	{
		JogDistanceCurveStartTime = 0;
		DistanceMachingLocation = Pawn->GetActorLocation();
	}
	else
	{
		JogDistanceCurveStopTime = 0;

		//TODO : check failed
		UAnimExtensionBlueprintLibrary::PredictStopLocation(
			DistanceMachingLocation,
			Pawn->GetActorLocation(),
			CharacterMovement->Velocity,
			CurrentAcceleration,
			CharacterMovement->BrakingFriction,
			CharacterMovement->GetMaxBrakingDeceleration(),
			CharacterMovement->MaxSimulationTimeStep,
			100);

		if (CVarDistanceMatchingDebug.GetValueOnAnyThread() == 1)
		{
			this->AnimInstanceProxy->AnimDrawDebugSphere(DistanceMachingLocation, 20.f, 12, FColor::Green, false, 2.0f, 1.0f);
		}
	}


}

void UAnimExtensionAnimInstance::EvalDistanceMatching(float DeltaTimeX)
{
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	if (!GetJogStartAnim() || !GetJogStopAnim())
		return;

	FVector Location = Pawn->GetActorLocation();
	float Distance = FVector::DistXY(Location, DistanceMachingLocation);
	float Time = 0;
	float* Target = nullptr;
	
	if (IsAccelerating)
	{
		
		Time = UAnimExtensionBlueprintLibrary::GetDistanceCurveTime(CurrentJogStartAnimSequence, Distance);
		Target = &JogDistanceCurveStartTime;
	}
	else
	{
		Time = UAnimExtensionBlueprintLibrary::GetDistanceCurveTime(CurrentJogStopAnimSequence, -Distance);
		Target = &JogDistanceCurveStopTime;
	}


	if (Time > *Target)
	{
		*Target = Time;
	}
	else
	{
		*Target += DeltaTimeX;
	}

	if (CVarDistanceMatchingDebug.GetValueOnAnyThread() == 1)
	{
		this->AnimInstanceProxy->AnimDrawDebugDirectionalArrow(Pawn->GetActorLocation(), DistanceMachingLocation, 1.0f, FColor::Green, false, 0.05f, 0.5f);
		UKismetSystemLibrary::DrawDebugString(this->AnimInstanceProxy->GetSkelMeshComponent(), this->AnimInstanceProxy->GetSkelMeshCompOwnerTransform().GetLocation(),
		FString::Printf(TEXT("Distance = %f \n JogStartTime = %f \n JogStopTime = %f"), Distance, JogDistanceCurveStartTime, JogDistanceCurveStopTime)
		);
	}
}

PRAGMA_ENABLE_OPTIMIZATION