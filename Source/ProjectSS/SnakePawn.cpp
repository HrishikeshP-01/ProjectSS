// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the root component
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(DefaultSceneComponent);
	// Initialize the arrow component
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Construction Script
void ASnakePawn::OnConstruction(const FTransform& Transform)
{
	if (CreateViaConstructionScript)
	{
		AddCollisionSpheres();
		AddSplineMeshes();
	}
	else { ClearIfNeeded(); }
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
	return CollisionSpheresList[0];
}

void ASnakePawn::AddCollisionSpheres()
{
	if (CollisionSpheresList.Num() != 0)
	{
		for (USphereComponent* Sphere : CollisionSpheresList)
		{
			if (IsValid(Sphere)) { Sphere->DestroyComponent(); }
		}
		CollisionSpheresList.Empty();
	}
	float CurrentSphereDistance = 0.0f, CurrentSphereRadius = 0.0f;
	for (int i = 0; i < NodesCount; i++)
	{
		USphereComponent* CurrentSphere = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
		CurrentSphere->RegisterComponent();
		CurrentSphere->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		CurrentSphereRadius = ((float)NodesCount * ScalingDownStartRatio < (float)i) ? CalculateRadius(i) : DefaultRadius;
		CurrentSphere->SetSphereRadius(CurrentSphereRadius, true);

		CurrentSphere->SetRelativeLocation(FVector(CurrentSphereDistance, 0.0f, 0.0f));
		CurrentSphere->SetCollisionProfileName(TEXT("PhysicsActor"));
		CurrentSphere->SetSimulatePhysics(true);
		// C++ equivalent of simulation generates hit events
		CurrentSphere->BodyInstance.bNotifyRigidBodyCollision = true;
		/*Properties like:
			Generate Overlap Events (true)
			Can Character Setp Up On (yes)
			Apply Impulse On Damage (true)
		etc. are set by default so I'm not going to specify them here
		*/

		// Update the distance for the next sphere to be added
		CurrentSphereDistance = CurrentSphereDistance + 2 * CurrentSphereRadius + ExtraSpacing;

		// Set physics properties of the sphere
		CurrentSphere->SetMassOverrideInKg(FName(TEXT("None")), CalculateMass(i, CurrentSphereRadius), true);
		CurrentSphere->SetLinearDamping(LinearDamping);
		CurrentSphere->SetAngularDamping(AngularDamping);
		CurrentSphere->SetPhysMaterialOverride(PhysicalMaterial);

		// Add it to the collision sphere array
		CollisionSpheresList.Add(CurrentSphere);

		// DEBUG
		CurrentSphere->SetVisibility(true);
		CurrentSphere->SetHiddenInGame(false);
	}
}

float ASnakePawn::CalculateRadius(int index)
{
	float alpha = FMath::Clamp(((float)index - (float)NodesCount * ScalingDownStartRatio) / ((float)NodesCount * (1 - ScalingDownStartRatio)), 0.0f, 1.0f);
	float lerpAns = FMath::Lerp(DefaultRadius, DefaultRadius * LastSphereRadiusRatio, alpha);
	return FMath::Max(lerpAns, 0.01f);
}

float ASnakePawn::CalculateMass(int index, float CurrentSphereRadius)
{
	float MassScale = (index == 0) ? HeadMassScale : 1.0f;
	return CurrentSphereRadius / DefaultRadius * DefaultMass * MassScale;
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