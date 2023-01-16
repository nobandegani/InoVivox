/* Copyright (c) 2021-2023 by Inoland */

#pragma once
#include "CoreMinimal.h"

#include "VivoxCoreCommon.h"

#include "IBpParticipant.generated.h"

UCLASS()
class UBpParticipant : public UObject
{
 GENERATED_BODY()

 IParticipant* Participant;

 UBpParticipant();
 UBpParticipant(const FObjectInitializer& ObjectInitializer);
 UBpParticipant(const FObjectInitializer& ObjectInitializer, IParticipant* Participant);
 UBpParticipant(IParticipant* Participant);
 
};