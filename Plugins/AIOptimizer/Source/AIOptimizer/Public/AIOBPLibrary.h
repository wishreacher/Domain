// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIOptimizerTypes.h"
#include "AIOBPLibrary.generated.h"


class ACharacter;

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class AIOPTIMIZER_API UAIOBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns true if subject handle is valid, for debugging */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "AIOLibrary")
	static bool IsHandleValid( const FAIOSubjectHandle& Handle ) { return Handle.IsHandleValid(); }

	/**
	 * Finds given handle in given array
	 * @return - INDEX_NONE if not found or index of found handle in array
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOLibrary" )
	static int32 FindHandle( const TArray<FAIOSubjectHandle>& Array, const FAIOSubjectHandle& HandleToFind ) { return Array.IndexOfByKey(HandleToFind); }

	/**
	 * Adds given Handle to given Array
	 * @return - INDEX_NONE if item was already in the Array, or added index if it was just added
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOLibrary" )
	static int32 AddUniqueHandle( UPARAM(ref) TArray<FAIOSubjectHandle>& Array, const FAIOSubjectHandle& Handle );

	/**
	 * Removes given handle from given array
	 * @return - True if handle was found and removed, false if handle couldn't be found
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOLibrary" )
	static bool RemoveHandle( UPARAM( ref ) TArray<FAIOSubjectHandle>& Array, const FAIOSubjectHandle& Handle );

	/** Converts subject handle to string, for debugging */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "AIOLibrary" )
	static FString GetString( const FAIOSubjectHandle& Handle ) { return Handle.ToString(); }

	/** Disable/Enable logic of AI to optimize it */
	UFUNCTION( BlueprintCallable, Category = "AIOLibrary" )
	static void SetAILogicEnabled( AActor* Actor, bool bEnable );

	/** Disable/Enable character movement component to optimize it */
	UFUNCTION( BlueprintCallable, Category = "AIOLibrary" )
	static void SetCharacterMovementEnabled( ACharacter* Character, bool bEnable );

	/** Returns "invoker" tag that assigned to every actor that has AIOSInvokerComponent */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "AIOLibrary" )
	static const FName& GetInvokerTag() { return INVOKER_TAG; }

	/** Returns "subject" tag that assigned to every actor that has AIOSubjectComponent */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "AIOLibrary" )
	static const FName& GetSubjectTag() { return SUBJECT_TAG; }

};
