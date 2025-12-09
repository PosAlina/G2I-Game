#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/G2IPipesSplineComponent.h"
#include "Components/G2IPipesBoxComponent.h"
#include "SplinesMetadata/G2IPipesSplineMetadata.h"
#include "Interfaces/G2IAirReciever.h"
#include "G2IPipe.generated.h"

UCLASS()
class G2I_API AG2IPipe : public AActor, public IG2IAirReciever
{
	GENERATED_BODY()
	
public:
	AG2IPipe();

	// Generates Meshes, Interactable Objects & Pipes Connections
	// TODO: SPLIT LOGIC IN FUNCTIONS TO BE ABLE TO CALL IT DURING RUNTIME
	void OnConstruction(const FTransform& Transform) override;

	void BeginPlay() override;

	// Interface function
	void RecieveAir_Implementation(AActor* sender, bool bAirPassed) override;

	UFUNCTION()
	void OnPipeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPipeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void AddActorToSendAirTo(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromSendAirTo(AActor* actor);

	// Passes Air to all actors in ActorsToSendAirTo.
	UFUNCTION(BlueprintCallable)
	void SendAir();

	// Getters & Setters

	TObjectPtr<UG2IPipesSplineMetadata> GetSplineMetadata() const;

	// If true - air is passed to that pipe (from other pipes or whatever).
	UFUNCTION(BlueprintCallable)
	bool GetHasAirPassed() const;

	UFUNCTION(BlueprintCallable)
	void SetHasAirPassed(bool newAirPassed);

	// If true - air is passing through pipe.
	// ! Call this function if you want to know if air is reaching next pipe or some other object.
	UFUNCTION(BlueprintCallable)
	bool GetAir() const;

	UFUNCTION(BlueprintCallable)
	float GetTestFloatAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetHasPipeAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetHasValveAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetHasTechnicalHoleAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetCanInteractAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetSendToOtherPipeAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetResieveFromOtherPipeAtSplinePoint(int32 PointIndex);

private:
	UG2IPipesBoxComponent* SpawnPipesBoxComponent(int32 PointIndex, bool bRecieves);
	void SpawnHoleActor(int32 PointIndex);
	void SpawnValveActor(int32 PointIndex);
	void SpawnInteractableBoxComponent(int32 PointIndex);
	void GenerateMesh(UStaticMesh* Mesh, int32 PointIndex);
	void RegenerateMesh(UStaticMesh* Mesh, int32 PointIndex);
	FVector GetLocationBetweenPoints(int32 Point1, int32 Point2);

public:
	// If I couldn't fix spline metadata to show up in the Editor - use this
	// ! DO NOT CHANGE THE SIZE, it updates automatically when spline is edited.
	UPROPERTY(EditAnywhere, Category = "Spline")
	TArray<FG2IPipesSplinePointParams> PointParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> BrokenMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults", 
		meta=(ToolTip="Base actor for Valves. Defines interaction responses"))
	TObjectPtr<AActor> ValveActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults", 
		meta = (ToolTip = "Base actor for Technical Holes. Defines interaction responses"))
	TObjectPtr<AActor> HoleActor;

private:
	UPROPERTY(Instanced, Export)
	TObjectPtr<UG2IPipesSplineMetadata> SplineMetadata;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2IPipesSplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere)
	float CollisionBoxExtent = 15.f;

	UPROPERTY(EditAnywhere)
	bool bHasAirPassed = true;

	bool bCanAirPassThrough = true;
	TArray<UG2IPipesBoxComponent*> sendingBoxComponents;

	UPROPERTY(VisibleAnywhere)
	TArray <TObjectPtr<AActor>> ActorsToSendAirTo;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Spline")
	bool bShowDebugUpArrows = false;
#endif
};
