// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HelperClass.generated.h"

/**
 * 
 */
UCLASS()
class IB_TEST_API UHelperClass : public UObject
{
	GENERATED_BODY()
	
public:
	static FName ConvertToName(const FText& Elem);
	static TArray<FName> ConvertToNames(const TArray<FText>& Array);

};
