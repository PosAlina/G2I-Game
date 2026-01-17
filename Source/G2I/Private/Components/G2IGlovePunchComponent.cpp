#include "Components/G2IGlovePunchComponent.h"
#include "G2I.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UG2IGlovePunchComponent::UG2IGlovePunchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UG2IGlovePunchComponent::ActivatePunch()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogG2I, Warning, TEXT("Owner is null"));
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World){
		UE_LOG(LogG2I, Warning, TEXT("World is null"));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character){
		UE_LOG(LogG2I, Warning, TEXT("Owner is not a Character"));
		return;
	}
	
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh){
		UE_LOG(LogG2I, Warning, TEXT("Mesh is null"));
		return;
	}

	FVector PunchBoneLocation = Mesh->GetBoneLocation(PunchBoneName);
	
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		PunchBoneLocation,
		PunchArea,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		AActor::StaticClass(),
		TArray<AActor*>(),
		ActorsToDestroy
	);

	UKismetSystemLibrary::DrawDebugSphere(World, PunchBoneLocation, PunchArea, 12, FColor::Purple, true, 2.0f);
}

TArray<AActor*> UG2IGlovePunchComponent::GetActorsToDestroy()
{
	return ActorsToDestroy;
}

void UG2IGlovePunchComponent::GadgetActivation_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogG2I, Warning, TEXT("Owner is null"));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Owner);

	if (!Character){
		UE_LOG(LogG2I, Warning, TEXT("Owner is not a Character"));
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();

	if (!Mesh){
		UE_LOG(LogG2I, Warning, TEXT("Mesh is null"));
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();

	if (!AnimInstance){
		UE_LOG(LogG2I, Warning, TEXT("AnimInstance is null"));
		return;
	}
	
	AnimInstance->Montage_Play(GlovePunchAnimMontage);
	
}

void UG2IGlovePunchComponent::ClearActorsToDestroy()
{
	ActorsToDestroy.Empty();
}
