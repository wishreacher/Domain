// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN( LogAIOptimizer, Log, All );


class FAIOptimizerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
