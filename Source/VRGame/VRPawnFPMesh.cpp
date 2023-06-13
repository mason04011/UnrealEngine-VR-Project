// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawnFPMesh.h"

#include "InteractableActor.h"
#include "XRMotionControllerBase.h"

class AInteractableActor;
// Sets default values
AVRPawnFPMesh::AVRPawnFPMesh()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	HMD = CreateDefaultSubobject<UCameraComponent>(TEXT("HMDCamera"));

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerArms"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	this->Arms->SetSkeletalMesh(ArmSkeletalMesh.Object);

	LeftHandObjectComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandObjectComponent"));
	LeftHandObjectComponent->SetupAttachment(Arms, TEXT("hand_l_socket"));

	RightHandObjectComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandObjectComponent"));
	RightHandObjectComponent->SetupAttachment(Arms, TEXT("hand_r_socket"));

	Arms->SetupAttachment(LeftController, "hand_l");
	Arms->SetupAttachment(RightController, "hand_r");
	
	RootComponent = VRTrackingCenter;
	HMD->SetupAttachment(VRTrackingCenter);
	LeftController->SetupAttachment(VRTrackingCenter);
	RightController->SetupAttachment(VRTrackingCenter);

	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	LeftController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	RightController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);


	AutoReceiveInput = EAutoReceiveInput::Player0;
}

void AVRPawnFPMesh::MoveForward(float AxisValue)
{
	this->AddActorWorldOffset(this->GetActorForwardVector() * (AxisValue * Speed));
}

void AVRPawnFPMesh::MoveRight(float AxisValue)
{
	this->AddActorWorldOffset(this->GetActorRightVector() * (AxisValue * Speed));

}

void AVRPawnFPMesh::Turn(float AxisValue)
{
	this->AddActorLocalRotation(FRotator(0, AxisValue, 0));
}

void AVRPawnFPMesh::GripRightPressed()
{
	FVector LineStart = Arms->GetSocketLocation("hand_r");
	FVector LineEnd = LineStart + Arms->GetRightVector() * 300;

	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Emerald, true, -1, 0, 2);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, LineStart, LineEnd, ECC_Visibility, QueryParams))
	{
		AInteractableActor* HitActor = Cast<AInteractableActor>(HitResult.GetActor());

		//UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("Hit Result: Actor: %s, Component: %s"), *HitActor->GetName(), *HitResult.GetComponent()->GetName()));
		
		if (HitActor)
		{
			UPrimitiveComponent* RootOfHitActor = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (RootOfHitActor)
			{
				RootOfHitActor->SetSimulatePhysics(false);
				RootOfHitActor->SetEnableGravity(false);
			}
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			HitActor->AttachToComponent(RightHandObjectComponent, AttachmentRules);
			HitActor->SetOwner(this);
			ObjectInRightHand = HitActor;
		}
	}
}

void AVRPawnFPMesh::GripRightReleased()
{
	FlushPersistentDebugLines(GetWorld());
	if (ObjectInRightHand)
	{
		UPrimitiveComponent* RootOfHitActor = Cast<UPrimitiveComponent>(ObjectInRightHand->GetRootComponent());
		if (RootOfHitActor)
		{
			RootOfHitActor->SetSimulatePhysics(true);
			RootOfHitActor->SetEnableGravity(true);
		}
		ObjectInRightHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ObjectInRightHand->SetOwner(nullptr);
		ObjectInRightHand = nullptr;
	}
}

void AVRPawnFPMesh::GripLeftPressed()
{
}

void AVRPawnFPMesh::GripLeftReleased()
{
}

void AVRPawnFPMesh::TriggerRight()
{
}

void AVRPawnFPMesh::TriggerLeft()
{
}

// Called when the game starts or when spawned
void AVRPawnFPMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRPawnFPMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPawnFPMesh::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVRPawnFPMesh::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRPawnFPMesh::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AVRPawnFPMesh::Turn);

	PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AVRPawnFPMesh::GripRightPressed);
	PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AVRPawnFPMesh::GripRightReleased);

	PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AVRPawnFPMesh::GripLeftPressed);
	PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AVRPawnFPMesh::GripLeftReleased);

	PlayerInputComponent->BindAction("TriggerRight", IE_Pressed, this, &AVRPawnFPMesh::TriggerRight);
	PlayerInputComponent->BindAction("TriggerLeft", IE_Released, this, &AVRPawnFPMesh::TriggerLeft);
}


