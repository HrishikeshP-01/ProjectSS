// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/Material.h"
#include "Components/SphereComponent.h"
#include "Components/SplineMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
// For debug purposes remove this and all functionality pertaining to it before release
#include "DrawDebugHelpers.h"
#include "SnakePawn.generated.h"

UCLASS()
class PROJECTSS_API ASnakePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASnakePawn();

	// Construction script
	virtual void OnConstruction(const FTransform& Transform)override;

	// Fns to create snake
	UFUNCTION(BlueprintCallable)
		USphereComponent* GetHeadCollisionShape();
	UFUNCTION(BlueprintCallable)
		void AddCollisionSpheres();
	UFUNCTION(BlueprintCallable)
		bool AddPhysicsConstraints();
	UFUNCTION(BlueprintCallable)
		bool AddSplineMeshes();
	UFUNCTION(BlueprintCallable)
		void AddSplineComponent();
	UFUNCTION(BlueprintCallable)
		void ClearIfNeeded();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Fn to calculate the radius of the current collision sphere
	float CalculateRadius(int index);
	// Fn to calculate mass of the current collision sphere
	float CalculateMass(int index, float CurrentSphereRadius);

	// Fn to create snake mesh
	void UpdateSplineComponent();
	void UpdateSplineMeshes();

	// Debug Fns
	UFUNCTION(BlueprintCallable)
	void Debug();
	void SetSnakeMeshVisibility(bool IsVisible);
	void DrawDebugSpheres(bool IsVisible);
	void DrawDebugSpline();

private:
	// The default (root) scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* DefaultSceneComponent = nullptr;
	// Arrow component for direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* Arrow = nullptr;
	// Spline component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineComponent = nullptr;

public:
	// Debug variables
	// Line Scale
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		float DebugLineScale = 0.2f;
	// Line Color
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		FColor DebugLineColor = FColor(1.0f, 0.119676f, 0.0f, 1.0f);
	// Sphere Color
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		FLinearColor DebugSphereColor = FLinearColor(1.0f, 0.895769f, 0.0f, 1.0f);
	// Variable for debug On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool DebugOn = false;

	// Properties variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		bool CreateViaConstructionScript = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		bool ForceCreationOnBeginPlay = false; // Setting this to true with CreateViaConstructionScript = true i.e., creating the snake at runtime throws off the character a bit
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float DefaultRadius = 26.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float DefaultMass = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float HeadMassScale = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 NodesCount = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float ExtraSpacing = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta=(UIMin="0.0", UIMax="1.0"))
		float ScalingDownStartRatio = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta=(UIMin="0.0", UIMax="1.0"))
		float LastSphereRadiusRatio = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float LinearDamping = 1.75f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float AngularDamping = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float BaseTurnRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float BaseLookUpRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float MoveUpImpulse = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float MoveDownImpulse = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		UPhysicalMaterial* PhysicalMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float MoveForwardPower = 3500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float MoveRightPower = 4000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float SpeedBoostPower = 8000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		UStaticMesh* HeadMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		UStaticMesh* TailMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		UStaticMesh* BodyMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		TArray<UMaterial*> BodyMaterialsList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		TArray<USphereComponent*> CollisionSpheresList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		TArray<USplineMeshComponent*> SplineMeshesList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		TArray<UPhysicsConstraintComponent*> PhysicsConstraintsList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float CurrentStretchRatio = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float CurrentRightTriggerAxis = 0.0f;

};
