#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IInvisibleForCharactersComponent.generated.h"

/**
 * To make an actor invisible to a specific characters,
 * you need to add this component to the actor
 * and select in Characters which characters this actor will not see.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IInvisibleForCharactersComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Characters that do not see owner of this component"))
	TArray<TSubclassOf<ACharacter>> InvisibleFromCharacters;

private:

	UPROPERTY()
	TObjectPtr<AActor> Owner;

public:

	virtual void BeginPlay() override;
	
private:

	void BindingToDelegates();

	void SetupDefaults();

	UFUNCTION()
	void HideActorsForCharacter(APawn *Pawn);

	UFUNCTION()
	void ShowActorsForCharacter(APawn *Pawn);
	
};
