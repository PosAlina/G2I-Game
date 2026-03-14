#include "Components/G2IOutlineComponent.h"

#include "G2I.h"

void UG2IOutlineComponent::OutlineController(bool bOutlineMode)
{
	AActor* Parent = GetOwner();
	TArray<UStaticMeshComponent*> OutlineMeshes;

	if (!ensure(Parent))
	{
		UE_LOG(LogG2I, Error, TEXT("Component %s does not have parent"), *GetName());
		return;
	}
	
	Parent->GetComponents<UStaticMeshComponent>(OutlineMeshes);

	for (auto OutlineMesh : OutlineMeshes)
	{
		if (!ensure(OutlineMesh))
		{
			UE_LOG(LogG2I, Error, TEXT("OutlineMesh in %s is null"), *Parent->GetName());
			return;
		}
		
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
