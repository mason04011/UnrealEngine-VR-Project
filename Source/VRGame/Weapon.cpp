// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DrawDebugHelpers.h"
#include "MovieSceneSequenceID.h"
#include "ParticleEmitterInstances.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	GunMesh->SetSkeletalMesh(GunSkeletalMesh.Object);
	RootComponent = GunMesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh, FName("Muzzle"));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void AWeapon::PrimaryAction()
{
	Super::PrimaryAction();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return;

	FVector Location;
	FRotator Rotation;
	if (GunMesh->DoesSocketExist("Muzzle"))
	{
		Location = GunMesh->GetSocketLocation("Muzzle");
		Rotation = GunMesh->GetSocketRotation("Muzzle");
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Location, Rotation, FVector::OneVector,true);


	FVector End = Location + Rotation.Vector() * MaxRange;

	//DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Cyan, true);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
	if (bSuccess)
	{
		FVector ShotDirection = -Rotation.Vector();
		//DrawDebugPoint(GetWorld(), Hit.Location, 5, FColor::Cyan, true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation(), FVector::OneVector);

		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PEW!"));
}
