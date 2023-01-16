/* Copyright (c) 2021-2023 by Inoland */

#include "IBpParticipant.h"
#include "VivoxCore.h"

UBpParticipant::UBpParticipant()
{
}

UBpParticipant::UBpParticipant(const FObjectInitializer& ObjectInitializer)
{
}

UBpParticipant::UBpParticipant(const FObjectInitializer& ObjectInitializer, IParticipant* Participant)
	: Participant(Participant)
{
}

UBpParticipant::UBpParticipant(IParticipant* Participant)
: Participant(Participant)
{
}
