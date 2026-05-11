#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2ITransitBetweenLevelsComponent.generated.h"

enum class EG2ILevelName : uint8;
class UG2IGameInstance;

UENUM()
enum class EG2ITransitBetweenLevelType : uint8
{
	TransitNextLevel,
	TransitMainMenu,
	TransitByName,
	TransitByIndex
};

UCLASS()
class G2I_API UG2ITransitBetweenLevelsComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere)
	TSet<TObjectPtr<APawn>> BanCharacters;

protected:

	UPROPERTY(EditAnywhere)
	EG2ITransitBetweenLevelType TransitType = EG2ITransitBetweenLevelType::TransitNextLevel;

	UPROPERTY(EditAnywhere)
	EG2ILevelName TargetLevelName;

	UPROPERTY(EditAnywhere)
	uint32 TargetLevelIndex;
	
private:

	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
public:

	bool Transit() const;
	
	virtual void BeginPlay() override;
};
