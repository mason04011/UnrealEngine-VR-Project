// Fill out your copyright notice in the Description page of Project Settings.


#include "ARWeapon.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AARWeapon::AARWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	GunMesh->SetSkeletalMesh(GunSkeletalMesh.Object);
	RootComponent = GunMesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh, FName("Muzzle"));

}

// Called when the game starts or when spawned
void AARWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bPrimaryFireActive)
	{
		Fire();
		// Delay between shots

	}
}


void AARWeapon::PrimaryAction()
{
    Super::PrimaryAction();

	bPrimaryFireActive = true;
    
}

void AARWeapon::StopPrimaryAction()
{
	Super::StopPrimaryAction();

	bPrimaryFireActive = false;
}

void AARWeapon::Fire()
{
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


		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
		if (bSuccess)
		{
			FVector ShotDirection = Muzzle->GetForwardVector();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation(), FVector::OneVector);

			AActor* HitActor = Hit.GetActor();
			if (HitActor != nullptr)
			{
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
			}
		}


	
}





