#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/G2IPipesSplineComponent.h"
#include "Components/G2IPipesBoxComponent.h"
#include "Gameplay/G2ITechnicalHole.h"
#include "Gameplay/G2IValve.h"
#include "SplinesMetadata/G2IPipesSplineMetadata.h"
#include "Interfaces/G2IAirRecieverInterface.h"
#include "G2IPipe.generated.h"

UCLASS(Blueprintable, Placeable)
class G2I_API AG2IPipe : public AActor, public IG2IAirRecieverInterface
{
	GENERATED_BODY()
	
public:
	AG2IPipe();

	// Generates Meshes, Interactable Objects & Pipes Connections
	void OnConstruction(const FTransform& Transform) override;

	void BeginPlay() override;

	// Interface function
	void RecieveAir_Implementation(AActor* Sender, bool bAirPassed) override;

	UFUNCTION()
	void OnPipeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPipeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void AddActorToSendAirTo(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromSendAirTo(AActor* Actor);

	// Passes Air to all actors in ActorsToSendAirTo.
	UFUNCTION(BlueprintCallable)
	void SendAir();

	void OnValveActivationChanged(AG2IValve* Valve, bool bNewActivated);

	// Getters & Setters

	TObjectPtr<UG2IPipesSplineMetadata> GetSplineMetadata() const;

	UFUNCTION(BlueprintCallable)
	void ChangeAirPassed(bool bNewAirPassed);

	UFUNCTION(BlueprintCallable)
	void ChangeCanAirPass(bool bNewCanAirPass);

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
	void SetHasTechnicalHoleAtSplinePoint(int32 PointIndex, bool bNewHasTechnicalHole);

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
	void SpawnValves();
	void SpawnTechnicalHoles();

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

	UPROPERTY(EditAnywhere, Category = "Pipe|Debug")
	float CollisionBoxExtent = 15.f;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	bool bHasAirPassed = true;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	bool bCanAirPassThrough = true;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	TArray <TObjectPtr<UG2IPipesBoxComponent>> SendingBoxComponents;

	// Array of Spline Mesh Components for swaping meshes at certain points
	// during runtime and not regenerating whole spline
	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	TMap<TObjectPtr<AG2IValve>, bool> ValvesMap;
	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	TSet<TObjectPtr<AG2ITechnicalHole>> HolesSet;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	TArray<TObjectPtr<AActor>> ActorsToSendAirTo;

	UPROPERTY(VisibleAnywhere, Category = "Pipe|Debug")
	TMap<TObjectPtr<AActor>, bool> ReceiveAirMap;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Spline|Debug")
	bool bShowDebugUpArrows = false;
#endif
};
