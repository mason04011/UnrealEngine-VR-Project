// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"

#include "Gun.h"
#include "InteractableActor.h"
#include "XRMotionControllerBase.h"


// Sets default values	
AVRPawn::AVRPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRTrackingCenter = CreateDefaultSubobject<USceneComponent>(TEXT("VRTrackingCenter"));
	HMD = CreateDefaultSubobject<UCameraComponent>(TEXT("HMDCamera"));

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHand"));
	LeftHand->SetRelativeRotation(FRotator(0, -90, 0));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> LeftHandMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Collections/LeftHand.LeftHand'"));
	this->LeftHand->SetSkeletalMesh(LeftHandMesh.Object);
	
	LeftHandObjectComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandObjectComponent"));
	LeftHandObjectComponent->SetupAttachment(LeftHand);



	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHand"));
	RightHand->SetRelativeRotation(FRotator(0, -90, 0));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RightHandMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Collections/RightHand.RightHand'"));
	this->RightHand->SetSkeletalMesh(RightHandMesh.Object);

	RightHandObjectComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandObjectComponent"));
	RightHandObjectComponent->SetupAttachment(RightHand);

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->InitCapsuleSize(30.0f, 96.0f);
	CollisionCapsule->SetCollisionProfileName("Pawn");
	CollisionCapsule->SetGenerateOverlapEvents(true);
	CollisionCapsule->CanCharacterStepUpOn = ECB_No;
	CollisionCapsule->SetShouldUpdatePhysicsVolume(true);
	CollisionCapsule->SetCanEverAffectNavigation(false);
	CollisionCapsule->SetupAttachment(VRTrackingCenter);
	CollisionCapsule->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));



	RootComponent = VRTrackingCenter;
	HMD->SetupAttachment(VRTrackingCenter);
	LeftController->SetupAttachment(VRTrackingCenter);
	LeftHand->SetupAttachment(LeftController);
	RightController->SetupAttachment(VRTrackingCenter);
	RightHand->SetupAttachment(RightController);

	LeftController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	RightController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	LeftController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);
	RightController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);

	AutoReceiveInput = EAutoReceiveInput::Player0;
}


// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVRPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AVRPawn::Turn);

	PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AVRPawn::GripRightPressed);
	PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AVRPawn::GripRightReleased);

	PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AVRPawn::GripLeftPressed);
	PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AVRPawn::GripLeftReleased);

	PlayerInputComponent->BindAction("TriggerRight", IE_Pressed, this, &AVRPawn::TriggerRightPressed);
	PlayerInputComponent->BindAction("TriggerRight", IE_Released, this, &AVRPawn::TriggerRightReleased);
	
	PlayerInputComponent->BindAction("TriggerLeft", IE_Pressed, this, &AVRPawn::TriggerLeftPressed);
	PlayerInputComponent->BindAction("TriggerLeft", IE_Released, this, &AVRPawn::TriggerLeftReleased);

}

void AVRPawn::MoveForward(float AxisValue)
{
	this->AddActorWorldOffset(this->GetActorForwardVector() * (AxisValue * Speed));
	//PlayerVelocity.X = AxisValue * Speed;	
}

void AVRPawn::MoveRight(float AxisValue)
{
	this->AddActorWorldOffset(this->GetActorRightVector() * (AxisValue * Speed));
}

void AVRPawn::Turn(float AxisValue)
{
	this->AddActorLocalRotation(FRotator(0, AxisValue, 0));
}


void AVRPawn::GripRightPressed()
{
	FVector LineStart = RightHand->GetComponentLocation();
	FVector LineEnd = LineStart + RightHand->GetRightVector() * 300;

	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Emerald, true, -1, 0, 2);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, LineStart, LineEnd, ECC_Visibility, QueryParams))
	{
		AInteractableActor* HitActor = Cast<AInteractableActor>(HitResult.GetActor());
		
		if (HitActor)
		{
			UPrimitiveComponent* RootOfHitActor = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (RootOfHitActor)
			{
				RootOfHitActor->SetSimulatePhysics(false);
				RootOfHitActor->SetEnableGravity(false);
			}
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			HitActor->AttachToComponent(RightHandObjectComponent,AttachmentRules);
			HitActor->SetOwner(this);
			ObjectInRightHand = HitActor;
		}
	}
}

void AVRPawn::GripRightReleased()
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

void AVRPawn::GripLeftPressed()
{
	FVector LineStart = LeftHand->GetComponentLocation();
	FVector LineEnd = LineStart + LeftHand->GetRightVector() * 300;

	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Emerald, true, -1, 0, 2);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, LineStart, LineEnd, ECC_Visibility, QueryParams))
	{
		AInteractableActor* HitActor = Cast<AInteractableActor>(HitResult.GetActor());

		if (HitActor)
		{
			UPrimitiveComponent* RootOfHitActor = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (RootOfHitActor)
			{
				RootOfHitActor->SetSimulatePhysics(false);
				RootOfHitActor->SetEnableGravity(false);
			}
			//HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			HitActor->AttachToComponent(LeftHandObjectComponent, AttachmentRules);
			HitActor->SetOwner(this);
			ObjectInLeftHand = HitActor;
		}
	}


}

void AVRPawn::GripLeftReleased()
{
	FlushPersistentDebugLines(GetWorld());
	if (ObjectInLeftHand)
	{
		UPrimitiveComponent* RootOfHitActor = Cast<UPrimitiveComponent>(ObjectInLeftHand->GetRootComponent());
		if (RootOfHitActor)
		{
			RootOfHitActor->SetSimulatePhysics(true);
			RootOfHitActor->SetEnableGravity(true);
		}
		ObjectInLeftHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ObjectInLeftHand->SetOwner(nullptr);
		ObjectInLeftHand = nullptr;
	}

}

void AVRPawn::TriggerRightPressed()
{
		if(ObjectInRightHand)
		{
			AInteractableActor* ItemHeld = Cast<AInteractableActor>(ObjectInRightHand);
			ItemHeld->PrimaryAction();
		}
		
}

void AVRPawn::TriggerLeftPressed()
{
	if(ObjectInLeftHand)
	{
		AInteractableActor* ItemHeld = Cast<AInteractableActor>(ObjectInLeftHand);
		ItemHeld->PrimaryAction();
	}
}

void AVRPawn::TriggerRightReleased()
{
	if(ObjectInRightHand)
	{
		AInteractableActor* ItemHeld = Cast<AInteractableActor>(ObjectInRightHand);
		ItemHeld->StopPrimaryAction();
	}
}

void AVRPawn::TriggerLeftReleased()
{
	if(ObjectInLeftHand)
	{
		AInteractableActor* ItemHeld = Cast<AInteractableActor>(ObjectInLeftHand);
		ItemHeld->StopPrimaryAction();
	}
}





