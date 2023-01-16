/* Copyright (c) 2021-2022 by Inoland */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FInoVivoxModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

