// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

class AGun;

UCLASS()
class VRGAME_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);

	void GripRightPressed();
	void GripRightReleased();
	void GripLeftPressed();
	void GripLeftReleased();

	void TriggerRightPressed();
	void TriggerLeftPressed();
	void TriggerRightReleased();
	void TriggerLeftReleased();



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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCapsuleComponent* CollisionCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UMotionControllerComponent* LeftController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UMotionControllerComponent* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	UStaticMeshComponent* Gun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USkeletalMeshComponent* LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USkeletalMeshComponent* RightHand;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* RightHandObjectComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* LeftHandObjectComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "VR")
	AActor* ObjectInRightHand = nullptr;

	UPROPERTY(EditAnywhere, Category = "VR")
	AActor* ObjectInLeftHand = nullptr;

	
	

};
