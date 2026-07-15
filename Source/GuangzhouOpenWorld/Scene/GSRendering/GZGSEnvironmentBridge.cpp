#include "Scene/GSRendering/GZGSEnvironmentBridge.h"

UGZGSEnvironmentBridge::UGZGSEnvironmentBridge()
{
}

void UGZGSEnvironmentBridge::Initialize()
{
	Humidity01 = 0.35f;
	WindSpeed = 2.0f;
	LightIntensity = 1.0f;
	RainIntensity = 0.0f;
	TimeOfDay = 12.0f;
	RecalculateParameters();
}

void UGZGSEnvironmentBridge::SetEnvironment(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity, float InTimeOfDay)
{
	Humidity01 = FMath::Clamp(InHumidity01, 0.0f, 1.0f);
	WindSpeed = FMath::Max(InWindSpeed, 0.0f);
	LightIntensity = FMath::Max(InLightIntensity, 0.0f);
	RainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);
	TimeOfDay = FMath::Fmod(InTimeOfDay, 24.0f);

	RecalculateParameters();
}

void UGZGSEnvironmentBridge::RecalculateParameters()
{
	// 湿度与降雨共同决定表面湿润度
	Parameters.Wetness = FMath::Clamp(Humidity01 * 0.4f + RainIntensity * 0.7f, 0.0f, 1.0f);

	// 湿度高时GS透明度略降，模拟空气中的水汽散射
	Parameters.Opacity = FMath::Lerp(1.0f, 0.92f, Parameters.Wetness);

	// 阴天/雨天降低饱和度，晴天保持
	Parameters.Saturation = FMath::Lerp(1.0f, 0.82f, Parameters.Wetness);

	// 风驱动植被/招牌GS位移
	Parameters.WindDisplacement = FMath::Clamp(WindSpeed * 0.5f, 0.0f, 15.0f);

	// 湿润路面反射增强
	Parameters.ReflectionIntensity = FMath::Lerp(0.3f, 1.0f, Parameters.Wetness);

	// 昼夜色温：夜晚偏暖，正午偏冷
	const float DayRatio = FMath::Sin((TimeOfDay / 24.0f) * PI * 2.0f - PI * 0.5f) * 0.5f + 0.5f;
	Parameters.ColorTemperature = FMath::Lerp(3200.0f, 7500.0f, DayRatio);

	// 光照强度受降雨/云层削弱
	Parameters.LightIntensityScale = LightIntensity * FMath::Lerp(1.0f, 0.55f, RainIntensity);
}
