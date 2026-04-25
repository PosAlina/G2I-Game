#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IGalleryWidget.generated.h"

class UImage;
class UButton;

UENUM(BlueprintType)
enum class EG2IGallerySectionsName : uint8
{
	Locations,
	Environment,
	MainCharacters,
	MinorCharacters,
	Puzzles
};

USTRUCT(BlueprintType)
struct FG2IGallerySectionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UTexture2D>> SectionImages;
};

/**
 * 
 */
UCLASS()
class G2I_API UG2IGalleryWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CurrentImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PreviousButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LocationsGalleryButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EnvironmentGalleryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainCharactersGalleryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MinorCharactersGalleryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PuzzlesGalleryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

private:

	EG2IGallerySectionsName CurrentSection = EG2IGallerySectionsName::Locations;

	int32 CurrentIndex = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EG2IGallerySectionsName, FG2IGallerySectionInfo> Sections;

public:

	TFunction<void()> OnBack;

protected:

	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void OnPreviousButtonClicked();

	UFUNCTION()
	void OnNextButtonClicked();

	UFUNCTION()
	void OnLocationsGalleryButtonClicked();
	
	UFUNCTION()
	void OnEnvironmentGalleryButtonClicked();
	
	UFUNCTION()
	void OnMainCharactersGalleryButtonClicked();

	UFUNCTION()
	void OnMinorCharactersGalleryButtonClicked();

	UFUNCTION()
	void OnPuzzlesGalleryButtonClicked();

private:

	void InitializeDefaults();
	void LockEmptySections() const;
	void OpenStartImage();
	void BindDelegates();

	bool IsSectionEmpty(EG2IGallerySectionsName SectionName) const;

	void OpenNewSection(EG2IGallerySectionsName NewSectionName);
	void OpenSection(EG2IGallerySectionsName SectionName);
	void OpenImage(int32 Index);
	bool SetTexture(UTexture2D* NewTexture) const;
};