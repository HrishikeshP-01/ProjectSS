// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Construction Script
void ASnakePawn::OnConstruction(const FTransform& Transform)
{

}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

USphereComponent* ASnakePawn::GetHeadCollisionShape()
{
	return nullptr;
}

void ASnakePawn::AddCollisionSpheres()
{

}

void ASnakePawn::AddPhysicsConstraints()
{

}

void ASnakePawn::AddSplineMeshes()
{

}

void ASnakePawn::AddSplineComponent()
{

}

void ASnakePawn::ClearIfNeeded()
{

}