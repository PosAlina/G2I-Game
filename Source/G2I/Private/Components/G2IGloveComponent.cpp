


#include "Components/G2IGloveComponent.h"

#include "G2IDestructibleActorBase.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UG2IGloveComponent::UG2IGloveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UG2IGloveComponent::BeginPlay()
{
	Super::BeginPlay();
	PC = GetWorld()->GetFirstPlayerController();
	// ...
	
}


// Called every frame
void UG2IGloveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UG2IGloveComponent::ActivatePunch()
{
	AActor* character = GetOwner();
	if (!character) return;
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	FVector PunchDirection = character->GetActorForwardVector();
	FVector PunchLocation = character->GetActorLocation() + PunchDirection * 50.f;
	PunchLocation.Z += 30;
	
	UKismetSystemLibrary::SphereOverlapActors(
		world,
		PunchLocation,
		PunchArea,
		TArray<TEnumAsByte<EObjectTypeQuery>>(), // Все типы объектов
		AActor::StaticClass(),
		TArray<AActor*>(),
		ActorsToDestroy
	);

	UKismetSystemLibrary::DrawDebugSphere(world, PunchLocation, PunchArea, 12, FColor::Blue, true, 2.0f);
}

TArray<AActor*> UG2IGloveComponent::GetActorsToDestroy()
{
	return ActorsToDestroy;
}

bool UG2IGloveComponent::IsGloveEnabled()
{
	return bIsGloveEnabled;
}

void UG2IGloveComponent::InputControl(bool isInputEnabled)
{
	if (PC)
	{
		isInputEnabled ? PC->EnableInput(PC) : PC->DisableInput(PC);
	}
}

void UG2IGloveComponent::GadgetActivation_Implementation()
{
	IG2IGadgetInterface::GadgetActivation_Implementation();

	if (bIsGloveEnabled)
	{
		OnGadgetActivationEvent.Broadcast();
	}
}

