#pragma once

#include "CoreMinimal.h"
#include "G2ILockingInterface.h"
#include "G2IWidgetNames.h"
#include "Components/WidgetComponent.h"
#include "G2IWorldHintWidgetComponent.generated.h"

class UG2IUserWidget;
class UG2IUIManager;
class AG2IPlayerController;
class USphereComponent;

UCLASS(ClassGroup=(Widgets), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IWorldHintWidgetComponent : public UWidgetComponent, public IG2ILockingInterface
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TMap<EG2IWidgetNames, TObjectPtr<UG2IUserWidget>> Widgets;

	EG2IWidgetNames CurrentWidgetName = EG2IWidgetNames::PointHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> VisibilityZone;
	
protected:

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;
	
	bool bIsLocked = false;

	UPROPERTY()
	TSubclassOf<APawn> PlayerPawnClass;
	
private:
	
	bool bIsInVisibleZone = false;
	bool bEnableScaleFromDistance = true;
	FVector2D DefaultDrawSize = FVector2D::ZeroVector;

public:
	
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

	UG2IUserWidget* FindOrAddWidgetByName(EG2IWidgetNames WidgetName);

	virtual void SetIsLocked_Implementation(bool bIsNewLocked) override;
	virtual bool IsLocked_Implementation() override;

	bool IsInVisibleZone() const;
	
	FVector2D GetDefaultDrawSize() const;
	bool IsEnableScaleFromDistance() const;
	void SetEnableScaleFromDistance(bool bInEnableScaleFromDistance);

protected:

	UG2IWorldHintWidgetComponent();

	UFUNCTION()
	void OnVisibilityZoneBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnVisibilityZoneEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void SetDefaultsByPlayerPawn(APawn *Pawn);

	void OpenWidget();

	void CloseWidget();
	
	void SetWidgetByName(EG2IWidgetNames WidgetName);

	void ReactWidgetOnOverlappingActors();
	
private:

	void PreInitializationVisibleZone();
	
	void SetupDefaults();
	void InitializationUIManager();
	void InitializationPlayerController();
	void InitializationVisibilityZone();

	void BindDelegates();

};