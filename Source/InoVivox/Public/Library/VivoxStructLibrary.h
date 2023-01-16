/* Copyright (c) 2021-2023 by Inoland */

#pragma once

#include "VivoxCore/Public/Channel3DProperties.h"
#include "VivoxStructLibrary.generated.h"

USTRUCT(BlueprintType)
struct FInoChannel3DProperties
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, DisplayName = "Audible Distance", Category = "3D Properties")
	int32 audibleDistance = 2700;

	UPROPERTY(BlueprintReadWrite, DisplayName = "Conversational Distance", Category = "3D Properties")
	int32 conversationalDistance = 90;

	UPROPERTY(BlueprintReadWrite, DisplayName = "Audio Fade Intensity By Distance", Category = "3D Properties")
	double audioFadeIntensityByDistance = 1.0;

	UPROPERTY(BlueprintReadWrite, DisplayName = "Audio Fade Mode", Category = "3D Properties")
	EAudioFadeModel audioFadeMode = EAudioFadeModel::InverseByDistance;

	
	FInoChannel3DProperties ()
		
	{}
};

USTRUCT(BlueprintType)
struct FInoParticipantProperties
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, DisplayName = "Is Self?", Category = "ParticipantProperties")
	bool IsSelf = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "In Audio?", Category = "ParticipantProperties")
	bool InAudio = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "Is Speech Detected?", Category = "ParticipantProperties")
	bool SpeechDetected = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "AudioEnergy", Category = "ParticipantProperties")
	double AudioEnergy = 0;

	UPROPERTY(BlueprintReadOnly, DisplayName = "Is Typing", Category = "ParticipantProperties")
	bool IsTyping = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "Is Muted For All", Category = "ParticipantProperties")
	bool IsMutedForAll = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "Unavailable Render Device", Category = "ParticipantProperties")
	bool UnavailableRenderDevice = false;

	UPROPERTY(BlueprintReadOnly, DisplayName = "Unavailable Capture Device", Category = "ParticipantProperties")
	bool UnavailableCaptureDevice = false;

	FInoParticipantProperties()
	{}
	
	FInoParticipantProperties (bool IsSelf, bool InAudio, bool SpeechDetected, double AudioEnergy, bool IsTyping, bool IsMutedForAll, bool UnavailableRenderDevice, bool UnavailableCaptureDevice)
		: IsSelf(IsSelf), InAudio(InAudio), SpeechDetected(SpeechDetected), AudioEnergy(AudioEnergy), IsTyping(IsTyping), IsMutedForAll(IsMutedForAll), UnavailableRenderDevice(UnavailableRenderDevice), UnavailableCaptureDevice(UnavailableCaptureDevice)
	{}

	FInoParticipantProperties (bool IsSelf, bool InAudio, bool SpeechDetected, double AudioEnergy, bool IsMutedForAll)
		: IsSelf(IsSelf), InAudio(InAudio), SpeechDetected(SpeechDetected), AudioEnergy(AudioEnergy), IsMutedForAll(IsMutedForAll)
	{}
};

