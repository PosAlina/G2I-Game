#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/G2IPipesSplineComponent.h"
#include "Components/G2IPipesBoxComponent.h"
#include "Gameplay/G2ITechnicalHole.h"
#include "Gameplay/G2IValve.h"
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

	void OnValveActivationChanged(AG2IValve* Valve, bool newActivated);

	// Getters & Setters

	TObjectPtr<UG2IPipesSplineMetadata> GetSplineMetadata() const;

	UFUNCTION(BlueprintCallable)
	void ChangeAirPassed(bool newAirPassed);

	UFUNCTION(BlueprintCallable)
	void ChangeCanAirPass(bool newCanAirPass);

	UFUNCTION(BlueprintCallable)
	void CheckIfAirCanPass();

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
	FRotator GetValveRotationAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetValveActivatedAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetHasTechnicalHoleAtSplinePoint(int32 PointIndex);
	UFUNCTION(BlueprintCallable)
	void SetHasTechnicalHoleAtSplinePoint(int32 PointIndex, bool newHasTechnicalHole);

	UFUNCTION(BlueprintCallable)
	bool GetCanInteractAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetSendToOtherPipeAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetReceiveFromOtherPipeAtSplinePoint(int32 PointIndex);

private:
	UG2IPipesBoxComponent* SpawnPipesBoxComponent(int32 PointIndex, bool bRecieves);
	void SpawnTechnicalHole(int32 PointIndex);
	void SpawnValve(int32 PointIndex);
	void SpawnInteractableBoxComponent(int32 PointIndex);
	void GenerateMesh(UStaticMesh* Mesh, int32 PointIndex);
	void RegenerateMesh(UStaticMesh* Mesh, int32 PointIndex);
	FVector GetLocationBetweenPoints(int32 Point1, int32 Point2, ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::Local);
	void ForceOverlaps();

public:
	// If I couldn't fix spline metadata to show up in the Editor - use this
	UPROPERTY(EditAnywhere, Category = "Spline", meta=(ToolTip="Parameters for spline points. DO NOT CHANGE THE SIZE, it updates automatically when spline is edited."))
	TArray<FG2IPipesSplinePointParams> PointParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> BrokenMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pipe",
		meta=(ToolTip="Base component for Valves. Defines interaction responses & other properties."))
	TSubclassOf<AG2IValve> ValveClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pipe",
		meta = (ToolTip = "Base actor for Technical Holes. Defines interaction responses & other properties."))
	TSubclassOf<AG2ITechnicalHole> HoleClass;

private:
	UPROPERTY(Instanced, Export)
	TObjectPtr<UG2IPipesSplineMetadata> SplineMetadata;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2IPipesSplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Pipe")
	float CollisionBoxExtent = 15.f;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	bool bHasAirPassed = true;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	bool bCanAirPassThrough = true;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	TArray<UG2IPipesBoxComponent*> sendingBoxComponents;

	// Array of Spline Mesh Components for swaping meshes at certain points
	// during runtime and not regenerating whole spline
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	TMap<TObjectPtr<AG2IValve>, bool> ValvesMap;
	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	TSet<TObjectPtr<AG2ITechnicalHole>> HolesSet;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	TArray<TObjectPtr<AActor>> ActorsToSendAirTo;

	UPROPERTY(VisibleAnywhere, Category = "Pipe")
	TMap<TObjectPtr<AActor>, bool> ReceiveAirMap;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Spline")
	bool bShowDebugUpArrows = false;
#endif
};
