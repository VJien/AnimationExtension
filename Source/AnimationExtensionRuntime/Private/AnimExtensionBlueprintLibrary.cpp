// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimExtensionBlueprintLibrary.h"

PRAGMA_DISABLE_OPTIMIZATION
bool UAnimExtensionBlueprintLibrary::PredictStopLocation(FVector& OutStopLocation, const FVector& CurrentLocation, const FVector& Velocity,
	const FVector& Acceleration, float Friction, float BrakingDeceleration, const float TimeStep, const int MaxSimulationIterations /*= 10*/)
{
	const float MIN_TICK_TIME = 1e-6;
	if (TimeStep < MIN_TICK_TIME)
	{
		return false;
	}
	// Apply braking or deceleration
	const bool bZeroAcceleration = Acceleration.IsZero();

	//���ٶȷ������ٶȷ���һ��
	if ((Acceleration | Velocity) > 0.0f)
	{
		return false;
	}

	BrakingDeceleration = FMath::Max(BrakingDeceleration, 0.f);
	Friction = FMath::Max(Friction, 0.f);
	const bool bZeroFriction = (Friction == 0.f);
	const bool bZeroBraking = (BrakingDeceleration == 0.f);

	if (bZeroAcceleration && bZeroFriction)
	{
		return false;
	}

	FVector LastVelocity = bZeroAcceleration ? Velocity : Velocity.ProjectOnToNormal(Acceleration.GetSafeNormal());
	LastVelocity.Z = 0;

	FVector LastLocation = CurrentLocation;

	int Iterations = 0;
	while (Iterations < MaxSimulationIterations)
	{
		Iterations++;

		const FVector OldVel = LastVelocity;

		// û�м��ٶ�
		if (bZeroAcceleration)
		{
			// subdivide braking to get reasonably consistent results at lower frame rates
			// (important for packet loss situations w/ networking)
			float RemainingTime = TimeStep;
			const float MaxTimeStep = (1.0f / 33.0f);

			// Decelerate to brake to a stop
			const FVector RevAccel = (bZeroBraking ? FVector::ZeroVector : (-BrakingDeceleration * LastVelocity.GetSafeNormal()));
			while (RemainingTime >= MIN_TICK_TIME)
			{
				// ��Ħ���Ļ���ʹ�����Գ�������
				const float dt = ((RemainingTime > MaxTimeStep && !bZeroFriction) ? FMath::Min(MaxTimeStep, RemainingTime * 0.5f) : RemainingTime);
				RemainingTime -= dt;

				// Ӧ��Ħ�����ͼ��ٶ�  Vt��Vo+at
				LastVelocity = LastVelocity + ((-Friction) * LastVelocity + RevAccel) * dt;

				// ��֤��Ҫ����
				if ((LastVelocity | OldVel) <= 0.f)
				{
					LastVelocity = FVector::ZeroVector;
					break;
				}
			}

			// С��һ���ٶȾ�����Ϊ0�ٶ�
			const float VSizeSq = LastVelocity.SizeSquared();
			if (VSizeSq <= 1.f || (!bZeroBraking && VSizeSq <= FMath::Square(10)))
			{
				LastVelocity = FVector::ZeroVector;
			}
		}
		else //�м��ٶ�
		{
			FVector TotalAcceleration = Acceleration;
			TotalAcceleration.Z = 0;

			// Ħ��Ӱ�����Ǹı䷽�����������ֻ������������ٶȣ�������·������
			const FVector AccelDir = TotalAcceleration.GetSafeNormal();
			const float VelSize = LastVelocity.Size();
			TotalAcceleration += -(LastVelocity - AccelDir * VelSize) * Friction;

			LastVelocity += TotalAcceleration * TimeStep;
		}

		LastLocation += LastVelocity * TimeStep;

		//�ٶ�С����ֵ������Ϊ0
		const float VSizeSq = LastVelocity.SizeSquared();
		if (VSizeSq <= 1.f
			|| (LastVelocity | OldVel) <= 0.f)
		{
			OutStopLocation = LastLocation;
			return true;
		}
	}

	return false;


}

float UAnimExtensionBlueprintLibrary::GetDistanceCurveTime(UAnimSequence* Sequence, float Distance, FName DistanceCurveName/* = "DistanceCurve"*/)
{
		if (!Sequence)
		{
			return 0;
		}
		FRawCurveTracks CurvesOfAnim = Sequence->GetCurveData();
		TArray<FFloatCurve> Curves = CurvesOfAnim.FloatCurves;

		for (int i = 0; i < Curves.Num(); i++)
		{
			if (Curves[i].Name.DisplayName == DistanceCurveName)
			{
				auto& Keys = Curves[i].FloatCurve.Keys;
				for (int j = 0; j < Keys.Num(); j++)
				{
					if (Keys[j].Value >= Distance)
					{
						float NextTime = Keys[j].Time;
						float NextValue = Keys[j].Value;
						float PrevValue = 0;
						float PrevTime = 0;
						if (j > 0)
						{
							PrevValue = Keys[j - 1].Value;
							PrevTime = Keys[j - 1].Time;
						}
						float Lerp = (Distance - PrevValue) / (NextValue - PrevValue);
						return PrevTime + (NextTime - PrevTime) * Lerp;
					}
				}
			}
		}

		return 0;
};

PRAGMA_ENABLE_OPTIMIZATION