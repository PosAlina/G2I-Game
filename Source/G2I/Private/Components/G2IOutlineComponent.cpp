#include "Components/G2IOutlineComponent.h"

void UG2IOutlineComponent::OutlineController(bool bOutlineMode)
{
	AActor* Parent = GetOwner();
	TArray<UStaticMeshComponent*> OutlineMeshes;
	if (Parent)
	{
		Parent->GetComponents<UStaticMeshComponent>(OutlineMeshes);
	}

	for (auto OutlineMesh : OutlineMeshes)
	{
		if (OutlineMesh)
		{
			OutlineMesh->bDisallowNanite = bOutlineMode;
			if (bOutlineMode)
			{
				OutlineMesh->SetOverlayMaterial(OutlineMaterialInstance);
			}
			else
			{
				OutlineMesh->SetOverlayMaterial(nullptr);
			}
		}
	}
}