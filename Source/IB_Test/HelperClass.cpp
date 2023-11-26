// Copyright Yoan Rock 2023. All Rights Reserved.

#include "HelperClass.h"

FName UHelperClass::ConvertToName(const FText& Elem)
{
	return FName(*Elem.ToString());
}

TArray<FName> UHelperClass::ConvertToNames(const TArray<FText>& Array)
{
	TArray<FName> Names;
	for(const auto& Elem : Array)
	{
		Names.Add(ConvertToName(Elem));
	}

	return Names;
}
