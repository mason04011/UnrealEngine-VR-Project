// Fill out your copyright notice in the Description page of Project Settings.


#include "LauncherWeapon.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALauncherWeapon::ALauncherWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AssetPacks/ShooterGame/Weapons/Launcher.Launcher'"));
	GunMesh->SetSkeletalMesh(GunSkeletalMesh.Object);
	RootComponent = GunMesh;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh, FName("Muzzle"));

}

// Called when the game starts or when spawned
void ALauncherWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALauncherWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALauncherWeapon::PrimaryAction()
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


