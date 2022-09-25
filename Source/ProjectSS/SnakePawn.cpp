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

		AddPhysicsConstraints();
		AddSplineComponent();
	}
	else { ClearIfNeeded(); }
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CreateViaConstructionScript || ForceCreationOnBeginPlay)
	{
		ClearIfNeeded();
		AddCollisionSpheres();
		AddSplineMeshes();
		AddPhysicsConstraints();
		AddSplineComponent();
	}
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSplineComponent();
	UpdateSplineMeshes();


	/*  
	------------------------
	For debugging
	Remove this before release
	------------------------
	*/
	if (DebugOn)
	{
		DrawDebugSpline();
	}
}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Inherit bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Can't use this method as the parameter we use in the function are const i.e., the value at the time of binding will be used throughout.
	   However am keeping this snippet as reference in case functions with parameters need to be binded in future.


	// Debug fn takes an input parameter to we need to follow a different procedure
	// Create delegate to connect with input binding
	FInputActionHandlerSignature ActionHandler;
	// Set delegate to the required fn and respective variable to be used as parameter
	ActionHandler.BindUFunction(this, TEXT("Debug"), DebugOn);
	// Preparing the Input binding
	FInputActionBinding* ActionBinding = new FInputActionBinding(FName("Debug"), IE_Pressed);
	// ActionBinding.KeyEvent = IE_Pressed;
	// Connect delegate with input binding
	ActionBinding->ActionDelegate = ActionHandler;
	// Bind action
	PlayerInputComponent->AddActionBinding(*ActionBinding);
	
		Instead of doing the above steps we could have made Debug without parameters.
		As DebugOn is a global variable we don't need it to be passed.
		However, I just wanted to learn how to bind action to functions with parameters.
		---------------------------------------
		SHOULD CLEAN THIS UP DURING PRODUCTION   
		WONT BE NEEDING DEBUG THEN ANYWAYS       
		---------------------------------------
	

	*/

	PlayerInputComponent->BindAction(FName("Debug"), IE_Pressed, this, &ASnakePawn::Debug);
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
		//CurrentSphere->SetVisibility(true);
		//CurrentSphere->SetHiddenInGame(false);
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

bool ASnakePawn::AddPhysicsConstraints()
{
	if (CollisionSpheresList.Num() <= 0) { return false; }

	// Destroy pre-existing constraints
	for (UPhysicsConstraintComponent* CurrentConstraint : PhysicsConstraintsList)
	{
		if (IsValid(CurrentConstraint)) { CurrentConstraint->DestroyComponent(); }
	}
	PhysicsConstraintsList.Empty();

	for (int i = 0; i < CollisionSpheresList.Num() - 1; i++)
	{
		UPhysicsConstraintComponent* CurrentConstraint = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());

		CurrentConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
		CurrentConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
		CurrentConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
		CurrentConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 50.0f);
		CurrentConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 50.0f);
		CurrentConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 50.0f);

		FVector loc = (CollisionSpheresList[i]->GetComponentLocation() + CollisionSpheresList[i + 1]->GetComponentLocation()) / 2.0f;
		CurrentConstraint->SetWorldLocation(loc);
		CurrentConstraint->SetConstrainedComponents(CollisionSpheresList[i], FName(TEXT("None")), CollisionSpheresList[i + 1], FName(TEXT("None")));

		PhysicsConstraintsList.Add(CurrentConstraint);

		// DEBUG
		CurrentConstraint->SetVisibility(true);
		CurrentConstraint->SetHiddenInGame(false);
	}

	return (PhysicsConstraintsList.Num() == NodesCount - 1) ? true : false;
}

bool ASnakePawn::AddSplineMeshes()
{
	if (CollisionSpheresList.Num() <= 0) { return false; }

	// Empty current spline mesh list
	for (USplineMeshComponent* SplineMesh : SplineMeshesList)
	{
		if (IsValid(SplineMesh)) { SplineMesh->DestroyComponent(); }
	}
	SplineMeshesList.Empty();

	for (int i = 0; i < CollisionSpheresList.Num()-1; i++)
	{
		USplineMeshComponent* CurrentSplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		CurrentSplineMesh->RegisterComponent();
		/*
		Getting Error: Root component is not static, can't attach SplineMeshComponent which is static
		Tried: 
				Setting root to static (breaks the game)
				Setting the SplineMeshComponent to Movable (no use)
		Solution:
				Didn't attach the SplineMeshComponent to anything

		CurrentSplineMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		CurrentSplineMesh->SetMobility(EComponentMobility::Movable);
		*/
		CurrentSplineMesh->SetCollisionProfileName(TEXT("NoCollision"));
		CurrentSplineMesh->SetEnableGravity(false);

		SplineMeshesList.Add(CurrentSplineMesh);

		CurrentSplineMesh->SetStartScale(FVector2D(CollisionSpheresList[i]->GetScaledSphereRadius() / 50.0f, CollisionSpheresList[i]->GetScaledSphereRadius() / 50.0f), true);
		CurrentSplineMesh->SetEndScale(FVector2D(CollisionSpheresList[i + 1]->GetScaledSphereRadius() / 50.0f, CollisionSpheresList[i + 1]->GetScaledSphereRadius() / 50.0f), true);
		CurrentSplineMesh->SetStartAndEnd(CollisionSpheresList[i]->GetComponentLocation(), GetActorForwardVector(), CollisionSpheresList[i+1]->GetComponentLocation(), GetActorForwardVector(), true);
	}

	// Set the meshes
	SplineMeshesList[0]->SetStaticMesh(HeadMesh);
	SplineMeshesList[NodesCount - 2]->SetStaticMesh(TailMesh);
	for (int i = 1; i < SplineMeshesList.Num() - 1; i++)
	{
		if (IsValid(SplineMeshesList[i])) { SplineMeshesList[i]->SetStaticMesh(BodyMesh); }
	}

	// Add code to set materials to mesh here if necessary
	//
	//

	return (SplineMeshesList.Num() == NodesCount - 1) ? true : false;
}

void ASnakePawn::AddSplineComponent()
{
	SplineComponent = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	//SplineComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//SplineComponent->SetMobility(EComponentMobility::Static);
	SplineComponent->SetAbsolute(true, true, true);
	SplineComponent->ClearSplinePoints();
}

void ASnakePawn::ClearIfNeeded()
{
	for (USphereComponent* CurrentSphere : CollisionSpheresList)
	{
		if (IsValid(CurrentSphere)) { CurrentSphere->DestroyComponent(); }
	}
	CollisionSpheresList.Empty();

	for (USplineMeshComponent* CurrentSplineMesh : SplineMeshesList)
	{
		if (IsValid(CurrentSplineMesh)) { CurrentSplineMesh->DestroyComponent(); }
	}
	SplineMeshesList.Empty();

	for (UPhysicsConstraintComponent* CurrentConstraint : PhysicsConstraintsList)
	{
		if (IsValid(CurrentConstraint)) { CurrentConstraint->DestroyComponent(); }
	}
	PhysicsConstraintsList.Empty();
}

void ASnakePawn::UpdateSplineComponent()
{
	SplineComponent->ClearSplinePoints();
	for (int i = 0;i < CollisionSpheresList.Num()-1;i++)
	{
		SplineComponent->AddSplinePoint(CollisionSpheresList[i]->GetComponentLocation(), ESplineCoordinateSpace::World, false);
	}
	SplineComponent->AddSplinePoint(CollisionSpheresList[CollisionSpheresList.Num() - 1]->GetComponentLocation(), ESplineCoordinateSpace::World, true);
}

void ASnakePawn::UpdateSplineMeshes()
{
	for (int i = 0; i < SplineMeshesList.Num(); i++)
	{
		FVector StartPosition, StartTangent, StopPosition, StopTangent;
		SplineComponent->GetLocationAndTangentAtSplinePoint(i, StartPosition, StartTangent, ESplineCoordinateSpace::Local);
		//StartPosition = StartPosition - StartTangent.Normalize(0.0001f) * CollisionSpheresList[i]->GetScaledSphereRadius();
		SplineComponent->GetLocationAndTangentAtSplinePoint(i+1, StopPosition, StopTangent, ESplineCoordinateSpace::Local);
		//StopPosition = StopPosition - StopTangent.Normalize(0.0001f) * CollisionSpheresList[i + 1]->GetScaledSphereRadius();

		SplineMeshesList[i]->SetStartAndEnd(StartPosition, StartTangent, StopPosition, StopTangent, false);

		FVector SplineUpVector = SplineComponent->GetUpVectorAtSplinePoint(i, ESplineCoordinateSpace::Local);
		SplineMeshesList[i]->SetSplineUpDir(SplineUpVector, true);
	}
}

// Debug Functions

void ASnakePawn::Debug()
{
	// Debug functionality here
	SetSnakeMeshVisibility(DebugOn);
	DrawDebugSpheres(DebugOn);

	DebugOn = !DebugOn;
}

void ASnakePawn::SetSnakeMeshVisibility(bool IsVisible)
{
	for (USplineMeshComponent* CurrentSplineMesh : SplineMeshesList)
	{
		if (IsValid(CurrentSplineMesh)) { CurrentSplineMesh->SetVisibility(IsVisible, false); /* Wont propogate to children */ }
	}
}

void ASnakePawn::DrawDebugSpheres(bool IsVisible)
{
	for (USphereComponent* CurrentSphere : CollisionSpheresList)
	{
		if (IsValid(CurrentSphere))
		{
			CurrentSphere->SetHiddenInGame(IsVisible); // Since we want it to do the opposite of the current DebugOn value
		}
	}
	/*
		To customize the spheres we can use DrawDebugSpheres() fn instead
		however this will do for now
	*/
}

void ASnakePawn::DrawDebugSpline()
{
	for (int i = 0;i < SplineComponent->GetNumberOfSplinePoints()-1;i++)
	{
		DrawDebugLine(GetWorld(), SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World), SplineComponent->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::World), DebugLineColor, false, 0.0f, 0, 10.0f * DebugLineScale);
	}
}
