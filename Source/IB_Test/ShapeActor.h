// Copyright Yoan Rock 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShapeActor.generated.h"

/**
 * Actor representing a Shape
 */
UCLASS()
class IB_TEST_API AShapeActor : public AActor
{
	GENERATED_BODY()

public:
	AShapeActor();
	
	virtual void Tick(float DeltaTime) override;

	/**
	 * @return The name of the shape.
	 */
	FName GetShapeName() const { return ShapeName;}

protected:
	/**
	 * The name of the shape.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shape")
	FName ShapeName = FName();

	/**
	 * The static mesh component representing the shape.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shape")
	UStaticMeshComponent* ShapeMesh;
};
