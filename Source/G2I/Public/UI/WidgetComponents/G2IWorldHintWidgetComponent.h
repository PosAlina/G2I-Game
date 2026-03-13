#pragma once

#include "CoreMinimal.h"
#include "G2IWidgetNames.h"
#include "Components/WidgetComponent.h"
#include "G2IWorldHintWidgetComponent.generated.h"

class UG2IUserWidget;
class UG2IUIManager;
class AG2IPlayerController;
class USphereComponent;

UCLASS(ClassGroup=(Widgets), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IWorldHintWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TMap<EG2IWidgetNames, TObjectPtr<UG2IUserWidget>> Widgets;

	EG2IWidgetNames CurrentWidgetName = EG2IWidgetNames::PointHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D WidgetSize = FVector2D(100.f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> VisibilityZone;
	
protected:

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;

public:

	void SetWidgetSize(FVector2D InWidgetSize);

	UG2IUserWidget* FindOrAddWidgetByName(EG2IWidgetNames WidgetName);

protected:

	UG2IWorldHintWidgetComponent();

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

	UFUNCTION()
	void OnVisibilityZoneBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnVisibilityZoneEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SetPlayerPawn(APawn *Pawn);

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