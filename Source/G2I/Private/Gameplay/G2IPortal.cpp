#include "Gameplay/G2IPortal.h"
#include "GameFramework/Character.h"
#include "Components/ArrowComponent.h"
#include "G2I.h"


// Sets default values
AG2IPortal::AG2IPortal()
{

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	RootComponent = Arrow;

}


void AG2IPortal::Teleport(ACharacter* Interactor)
{
    if (Interactor)
    {
        if (Portal && bIsActive && Portal->bIsActive && Portal->Arrow)
        {
            const FVector TargetLocation = Portal->Arrow->GetComponentLocation();
            const FRotator TargetRotation = Portal->Arrow->GetComponentRotation();

            Interactor->SetActorLocationAndRotation(Portal->Arrow->GetComponentLocation(), Portal->Arrow->GetComponentRotation());
        }

        if (Interactor->ActorHasTag(TEXT("Ghost")))
        {
            bIsActive = true;
        }
    }
    else
    {
        UE_LOG(LogG2I, Warning, TEXT("No interactor in %s"), *GetActorNameOrLabel());
    }
}
