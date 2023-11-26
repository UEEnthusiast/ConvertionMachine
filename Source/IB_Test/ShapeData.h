// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShapeData.generated.h"

class AShapeActor;

/**
 * Data structure containing all necessary information for a Shape.
 */
USTRUCT(BlueprintType)
struct IB_TEST_API FShapeData : public FTableRowBase
{
	GENERATED_BODY()

	FShapeData() = default;

	FShapeData(const FText& InName,const FText& InDescription, const TSubclassOf<AShapeActor> InShapeActorClass) :
	Name(InName), Description(InDescription), ShapeActorClass(InShapeActorClass)
	{
	}

	/**
	 * Name of the shape
	 */
	UPROPERTY(EditAnywhere)
	FText Name;

	/**
	 * Description of the shape
	 */
	UPROPERTY(EditAnywhere)
	FText Description;

	/**
	 * Class of the shape
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AShapeActor> ShapeActorClass;
};