// Copyright Yoan Rock 2023. All Rights Reserved.


#include "ShapeActor.h"

AShapeActor::AShapeActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ShapeMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ShapeMesh"));
	SetRootComponent(ShapeMesh);
}

void AShapeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RotateActor(DeltaTime);
}

void AShapeActor::RotateActor(float DeltaTime)
{
	FRotator NewRotation = GetActorRotation();
	float RotationSpeed = 15.0f; // Adjust the rotation speed as needed

	// Adjust the rotation based on the frame time and rotation speed
	NewRotation.Yaw += RotationSpeed * DeltaTime;

	// Set the new rotation
	SetActorRotation(NewRotation);
}

