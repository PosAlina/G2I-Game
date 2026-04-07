#include "Menu/Gallery/G2IGalleryWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UG2IGalleryWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
	BindDelegates();
}

void UG2IGalleryWidget::InitializeDefaults()
{
	LockEmptySections();
	OpenStartImage();
}

void UG2IGalleryWidget::LockEmptySections() const
{
	PreviousButton->SetIsEnabled(false);
	NextButton->SetIsEnabled(false);
	if (IsSectionEmpty(EG2IGallerySectionsName::Locations))
	{
		LocationsGalleryButton->SetIsEnabled(false);
	}
	if (IsSectionEmpty(EG2IGallerySectionsName::Environment))
	{
		EnvironmentGalleryButton->SetIsEnabled(false);
	}
	if (IsSectionEmpty(EG2IGallerySectionsName::MainCharacters))
	{
		MainCharactersGalleryButton->SetIsEnabled(false);
	}
	if (IsSectionEmpty(EG2IGallerySectionsName::MinorCharacters))
	{
		MinorCharactersGalleryButton->SetIsEnabled(false);
	}
	if (IsSectionEmpty(EG2IGallerySectionsName::Puzzles))
	{
		PuzzlesGalleryButton->SetIsEnabled(false);
	}
}

void UG2IGalleryWidget::OpenStartImage()
{
	if (LocationsGalleryButton->GetIsEnabled())
	{
		OpenSection(EG2IGallerySectionsName::Locations);
		return;
	}
	if (EnvironmentGalleryButton->GetIsEnabled())
	{
		OpenSection(EG2IGallerySectionsName::Environment);
		return;
	}
	if (MainCharactersGalleryButton->GetIsEnabled())
	{
		OpenSection(EG2IGallerySectionsName::MainCharacters);
		return;
	}
	if (MinorCharactersGalleryButton->GetIsEnabled())
	{
		OpenSection(EG2IGallerySectionsName::MinorCharacters);
		return;
	}
	if (PuzzlesGalleryButton->GetIsEnabled())
	{
		OpenSection(EG2IGallerySectionsName::Puzzles);
		return;
	}
}

void UG2IGalleryWidget::BindDelegates()
{
	if (ensure(BackButton))
	{
		BackButton->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find BackButton"), *GetName());
	}
	if (ensure(PreviousButton))
	{
		PreviousButton->OnClicked.AddDynamic(this, &ThisClass::OnPreviousButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find PreviousButton"), *GetName());
	}
	if (ensure(NextButton))
	{
		NextButton->OnClicked.AddDynamic(this, &ThisClass::OnNextButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find NextButton"), *GetName());
	}
	if (ensure(LocationsGalleryButton))
	{
		LocationsGalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnLocationsGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Locations Gallery"), *GetName());
	}
	if (ensure(EnvironmentGalleryButton))
	{
		EnvironmentGalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnEnvironmentGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Environment Gallery"), *GetName());
	}
	if (ensure(MainCharactersGalleryButton))
	{
		MainCharactersGalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnMainCharactersGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Main Characters Gallery"), *GetName());
	}
	if (ensure(MinorCharactersGalleryButton))
	{
		MinorCharactersGalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnMinorCharactersGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Minor Characters Gallery"), *GetName());
	}
	if (ensure(PuzzlesGalleryButton))
	{
		PuzzlesGalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnPuzzlesGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Puzzles Gallery"), *GetName());
	}
}

void UG2IGalleryWidget::OnBackButtonClicked()
{
	if (OnBack)
	{
		OnBack();
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Back function is undefined in %s"), *GetName());
	}
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseWidget(EG2IWidgetNames::Gallery);
}

void UG2IGalleryWidget::OnPreviousButtonClicked()
{
	OpenImage(CurrentIndex - 1);
}

void UG2IGalleryWidget::OnNextButtonClicked()
{
	OpenImage(CurrentIndex + 1);
}

void UG2IGalleryWidget::OpenImage(const int32 Index)
{
	const auto* SectionPtr = Sections.Find(CurrentSection);
	if (!SectionPtr || SectionPtr->SectionImages.IsEmpty())
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to select empty Gallery Section"), *GetName());
		return;
	}
	const int32 NewIndex = (Index + SectionPtr->SectionImages.Num()) % SectionPtr->SectionImages.Num();
	if (SetTexture(SectionPtr->SectionImages[NewIndex]))
	{
		CurrentIndex = NewIndex;
	}
}

bool UG2IGalleryWidget::SetTexture(UTexture2D* NewTexture) const
{
	if (!NewTexture)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to set null texture"), *GetName());
		return false;
	}
	CurrentImage->SetBrushFromTexture(NewTexture, true);
	return true;
}

bool UG2IGalleryWidget::IsSectionEmpty(const EG2IGallerySectionsName SectionName) const
{
	if (const auto *Section = Sections.Find(SectionName))
	{
		return Section->SectionImages.IsEmpty();
	}
	return true;
}

void UG2IGalleryWidget::OpenNewSection(const EG2IGallerySectionsName NewSectionName)
{
	if (CurrentSection == NewSectionName)
	{
		return;
	}
	OpenSection(NewSectionName);
}

void UG2IGalleryWidget::OpenSection(const EG2IGallerySectionsName SectionName)
{
	const auto* SectionPtr = Sections.Find(SectionName);
	if (!ensure(SectionPtr) || !ensure(!SectionPtr->SectionImages.IsEmpty()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Attempt to select empty Gallery Section"), *GetName());
		return;
	}
	
	if (SectionPtr->SectionImages.Num() > 1)
	{
		PreviousButton->SetIsEnabled(true);
		NextButton->SetIsEnabled(true);
	}
	else
	{
		PreviousButton->SetIsEnabled(false);
		NextButton->SetIsEnabled(false);
	}

	if (SetTexture(SectionPtr->SectionImages[0]))
	{
		CurrentSection = SectionName;
		CurrentIndex = 0;
	}
}

void UG2IGalleryWidget::OnLocationsGalleryButtonClicked()
{
	OpenNewSection(EG2IGallerySectionsName::Locations);
}

void UG2IGalleryWidget::OnEnvironmentGalleryButtonClicked()
{
	OpenNewSection(EG2IGallerySectionsName::Environment);
}

void UG2IGalleryWidget::OnMainCharactersGalleryButtonClicked()
{
	OpenNewSection(EG2IGallerySectionsName::MainCharacters);
}

void UG2IGalleryWidget::OnMinorCharactersGalleryButtonClicked()
{
	OpenNewSection(EG2IGallerySectionsName::MinorCharacters);
}

void UG2IGalleryWidget::OnPuzzlesGalleryButtonClicked()
{
	OpenNewSection(EG2IGallerySectionsName::Puzzles);
}
