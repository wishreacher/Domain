// Fill out your copyright notice in the Description page of Project Settings.


#include "GInstance.h"

bool UGInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if(!bResult)
	{
		TArray<UGameInstanceSubsystem*> SubSystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (UGameInstanceSubsystem* SubSystem : SubSystems)
		{
			bResult |= SubSystem->ProcessConsoleExec(Cmd, Ar, Executor);
		}
	}
	
	return bResult;
	
}
