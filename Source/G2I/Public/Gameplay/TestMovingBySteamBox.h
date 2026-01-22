

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestMovingBySteamBox.generated.h"

UCLASS()
class G2I_API ATestMovingBySteamBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestMovingBySteamBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
