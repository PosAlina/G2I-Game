#include "Gameplay/G2IDestructibleActorBase.h"
#include "G2I.h"
#include "G2ISoundComponent.h"
#include "NiagaraFunctionLibrary.h"

void AG2IDestructibleActorBase::DestroySelf_Implementation()
{
	if (bIsDestroyed)
	{
		return;
	}

	bIsDestroyed = true;

	const auto* World = GetWorld();
	
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	if (!DestroyedMesh)
	{
		UE_LOG(LogG2I, Log, TEXT("DestroyedMesh doesn't exist in %s"), *GetName());
	}
	else if (!DestructibleMeshComponent)
	{
		UE_LOG(LogG2I, Log, TEXT("Mesh doesn't exist in %s"), *GetName());
	}
	else
	{
		DestructibleMeshComponent->SetStaticMesh(DestroyedMesh);
	}

	if (!NiagaraSystem)
	{
		UE_LOG(LogG2I, Log, TEXT("NiagaraSystem doesn't exist in %s"), *GetName());
	}
	else
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World, 
				NiagaraSystem, 
				GetActorLocation()
			);
	}

	if (!ensure(SoundComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	
	SoundComponent->PlaySound(SoundID);
}

AG2IDestructibleActorBase::AG2IDestructibleActorBase()
{
	SoundComponent = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComp"));
	DestructibleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

	SetRootComponent(DestructibleMeshComponent);
}

void AG2IDestructibleActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensure(SoundComponent))
    {
    	UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
    	return;
    }

	const auto* SoundConf = SoundComponent->SetupSounds.Find(*SoundName);

	if (SoundConf)
	{
		SoundID = SoundComponent->AddSound(*SoundConf);
	}
}

