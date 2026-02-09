


#include "Gameplay/G2ISpawnLight.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/G2IInventoryComponent.h"

AG2ISpawnLight::AG2ISpawnLight()
{
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(Root);
    SpotLight->SetVisibility(false);
}

void AG2ISpawnLight::OnLight(AActor* Interactor)
{
    if (!Interactor)
        return;

    UG2IInventoryComponent* Inventory = Interactor->FindComponentByClass<UG2IInventoryComponent>();

    if (!Inventory)
        return;

    if (Inventory->HasItemID(RequiredItemID))
    {
        SpotLight->SetVisibility(true);

        Inventory->RemoveItemID(RequiredItemID);
    }

}
