#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "Components/ActorComponent.h"
#include "G2IPassThroughComponent.generated.h"

/**
 * Component of actor that the daughter can pass through
 */
UCLASS(ClassGroup=(CollisionAndAINavigation), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IPassThroughComponent : public UNavModifierComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AActor> Owner;

	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, int32> StaticMeshesMap;

	// This can't be UPROPERTY
	TMultiMap<int32, UMaterialInterface*> MaterialsMap;

	UPROPERTY(EditAnywhere, Category = Materials)
	TObjectPtr<UMaterialInterface> GhostMaterial;

public:
	
	UG2IPassThroughComponent();

protected:
	
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

private:

	void BindingToDelegates();

	void PreInitializeDefaults();

	UFUNCTION()
	void HideGhostMaterial(APawn* Pawn);

	UFUNCTION()
	void ShowGhostMaterial(APawn* Pawn);
};
