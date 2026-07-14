#include "GZSoLoudReverb.h"

UGZSoLoudReverb::UGZSoLoudReverb()
{
}

void UGZSoLoudReverb::SetMode(EGZSoLoudReverbMode Mode)
{
	CurrentMode = Mode;
}

EGZSoLoudReverbMode UGZSoLoudReverb::DetectMode(const FVector& Location, bool bIndoors, bool bEnclosed) const
{
	if (bEnclosed)
	{
		return EGZSoLoudReverbMode::Enclosed;
	}
	if (bIndoors)
	{
		return EGZSoLoudReverbMode::Indoor;
	}
	if (Location.Z < 50.0f)
	{
		return EGZSoLoudReverbMode::CityStreet;
	}
	return EGZSoLoudReverbMode::Outdoor;
}

FZSoLoudReverbParams UGZSoLoudReverb::GetCurrentParams() const
{
	FZSoLoudReverbParams Params;
	switch (CurrentMode)
	{
	case EGZSoLoudReverbMode::Indoor:
		Params.RoomSize = 0.6f;
		Params.Damping = 0.6f;
		Params.WetLevel = 0.25f * WeatherScale;
		Params.DecayTime = 1.2f;
		break;
	case EGZSoLoudReverbMode::Outdoor:
		Params.RoomSize = 2.0f;
		Params.Damping = 0.2f;
		Params.WetLevel = 0.05f * WeatherScale;
		Params.DecayTime = 0.4f;
		break;
	case EGZSoLoudReverbMode::CityStreet:
		Params.RoomSize = 1.4f;
		Params.Damping = 0.35f;
		Params.WetLevel = 0.15f * WeatherScale;
		Params.DecayTime = 0.9f;
		break;
	case EGZSoLoudReverbMode::Enclosed:
		Params.RoomSize = 0.4f;
		Params.Damping = 0.75f;
		Params.WetLevel = 0.45f * WeatherScale;
		Params.DecayTime = 2.0f;
		break;
	}
	return Params;
}

void UGZSoLoudReverb::SetWeatherWetLevelScale(float Scale)
{
	WeatherScale = FMath::Clamp(Scale, 0.0f, 2.0f);
}
