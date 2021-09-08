// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimExtensionAnimInstance.h"
#include "AnimExtensionBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"


void UAnimExtensionAnimInstance::NativeInitializeAnimation()
{
	//Very Important Line
	Super::NativeInitializeAnimation();
}

void UAnimExtensionAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	//Very Important Line
	Super::NativeUpdateAnimation(DeltaTimeX);

	UpdateDistanceMatching(DeltaTimeX);
	EvalDistanceMatching(DeltaTimeX);
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
	}
}

void UAnimExtensionAnimInstance::EvalDistanceMatching(float DeltaTimeX)
{
	APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
		return;

	if (!JogStartAnimSequence || !JogStopAnimSequence)
		return;

	FVector Location = Pawn->GetActorLocation();
	float Distance = FVector::DistXY(Location, DistanceMachingLocation);
	float Time = 0;
	float* Target = nullptr;

	if (IsAccelerating)
	{
		Time = UAnimExtensionBlueprintLibrary::GetDistanceCurveTime(JogStartAnimSequence, Distance);
		Target = &JogDistanceCurveStartTime;
	}
	else
	{
		Time = UAnimExtensionBlueprintLibrary::GetDistanceCurveTime(JogStopAnimSequence, -Distance);
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
}