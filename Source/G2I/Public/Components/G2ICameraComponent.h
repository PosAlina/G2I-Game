#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ICameraComponent.generated.h"


class UCameraComponent;
class USpringArmComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICameraComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UG2ICameraComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
