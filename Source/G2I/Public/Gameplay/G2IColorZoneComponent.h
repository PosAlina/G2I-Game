

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "G2IColorZoneComponent.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EZoneColor : uint8
{
	None UMETA(DisplayName = "None"),
	Red UMETA(DisplayName = "Red"),
	Blue UMETA(DisplayName = "Blue"),
	Yellow UMETA(DisplayName = "Yellow")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IColorZoneComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UG2IColorZoneComponent();
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EZoneColor Color = EZoneColor::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> ZoneCol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActivationZone = false;
};
