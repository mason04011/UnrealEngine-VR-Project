// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/SkinnedMeshComponent.h"
#include "VRPawnFPMesh.generated.h"

UCLASS()
class VRGAME_API AVRPawnFPMesh : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawnFPMesh();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);

	void GripRightPressed();
	void GripRightReleased();
	void GripLeftPressed();
	void GripLeftReleased();

	void TriggerRight();
	void TriggerLeft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USceneComponent* VRTrackingCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UCameraComponent* HMD;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UMotionControllerComponent* LeftController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UMotionControllerComponent* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UStaticMeshComponent* Gun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USkeletalMeshComponent* Arms;

	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* RightHandObjectComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* LeftHandObjectComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "VR")
	AActor* ObjectInRightHand = nullptr;

	UPROPERTY(EditAnywhere, Category = "VR")
	AActor* ObjectInLeftHand = nullptr;  

};
