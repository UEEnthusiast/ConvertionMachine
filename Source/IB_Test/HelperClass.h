// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HelperClass.generated.h"

/**
 * Helper class for various utility functions.
 */
UCLASS()
class IB_TEST_API UHelperClass : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Convert FText to FName.
	 *
	 * @param Elem The FText to convert.
	 * @return The corresponding FName.
	 */
	static FName ConvertToName(const FText& Elem);
	
	/**
	 * Convert an array of FText to an array of FName.
	 *
	 * @param Array The array of FText to convert.
	 * @return The corresponding array of FName.
	 */
	static TArray<FName> ConvertToNames(const TArray<FText>& Array);

};
