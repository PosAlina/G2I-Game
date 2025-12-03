#include "G2IPipesSplineMetadataDetails.h"

#include <IDetailGroup.h>
#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>
#include <Widgets/Input/SNumericEntryBox.h>
#include <ScopedTransaction.h>
#include <ComponentVisualizer.h>
#include <Editor.h>

#include "SplinesMetadata/G2IPipesSplineMetadata.h"

#define LOCTEXT_NAMESPACE "FG2IPipesSplineMetadataDetails"

/**
*   UCLASS G2IPipesSplineMetadataDetails
*/

TSharedPtr<ISplineMetadataDetails> UG2IPipesSplineMetadataDetails::Create()
{
    return MakeShared<FG2IPipesSplineMetadataDetails>();
}

UClass* UG2IPipesSplineMetadataDetails::GetMetadataClass() const
{
    return UG2IPipesSplineMetadataDetails::StaticClass();
}

/**
*   (press) F STRUCT G2IPipesSplineMetadataDetails
*/

FName FG2IPipesSplineMetadataDetails::GetName() const
{
    return FName(TEXT("G2IPipesSplineMetadataDetails"));
}

FText FG2IPipesSplineMetadataDetails::GetDisplayName() const
{
    return LOCTEXT("G2IPipesSplineMetadataDetails", "G2I");
}

template<class T>
bool UpdateMultipleValue(TOptional<T>& CurrentValue, T InValue)
{
	if (!CurrentValue.IsSet())
	{
		CurrentValue = InValue;
	}
	else if (CurrentValue.IsSet() && CurrentValue.GetValue() != InValue)
	{
		CurrentValue.Reset();
		return false;
	}

	return true;
}

void FG2IPipesSplineMetadataDetails::Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys)
{
	SplineComp = InSplineComponent;
	SelectedKeys = InSelectedKeys;
	//bHasPipeValue.Reset();
	TestFloatValue.Reset();

	if (InSplineComponent)
	{
		bool bUpdateTestFloat = true;

		UG2IPipesSplineMetadata* Metadata = Cast<UG2IPipesSplineMetadata>(InSplineComponent->GetSplinePointsMetadata());
		if (Metadata)
			for (int32 Index : InSelectedKeys)
			{
				if (Metadata->PointParams.IsValidIndex(Index) && bUpdateTestFloat)
				{
						bUpdateTestFloat = UpdateMultipleValue(TestFloatValue, Metadata->PointParams[Index].TestValue);
				}
			}
	}
}

void FG2IPipesSplineMetadataDetails::GenerateChildContent(IDetailGroup& DetailGroup)
{
	DetailGroup.AddWidgetRow()
		.Visibility(EVisibility::Visible)
		.NameContent()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.Text(LOCTEXT("TestFloatValue", "TestFloatValue"))
				.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(125.0f)
		.MaxDesiredWidth(125.0f)
		[
			SNew(SNumericEntryBox<float>)
				.Value(this, &FG2IPipesSplineMetadataDetails::GetTestFloat)
				.AllowSpin(false)
				.MinValue(0.0f)
				.MaxValue(TOptional<float>())
				.MinSliderValue(0.0f)
				.MaxSliderValue(TOptional<float>()) // No upper limit
				.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
				.OnValueCommitted(this, &FG2IPipesSplineMetadataDetails::OnSetTestFloat)
				.Font(IDetailLayoutBuilder::GetDetailFont())
		];
		/*
		[
			SNew(SCheckBox)
				.OnCheckStateChanged(this, &FG2IPipesSplineMetadataDetails::OnSetHasPipe)
				.CheckBoxContentUsesAutoWidth(true)
				.IsChecked(ECheckBoxState::Unchecked)
		];*/
}

UG2IPipesSplineMetadata* FG2IPipesSplineMetadataDetails::GetMetadata() const
{
	UG2IPipesSplineMetadata* Metadata = SplineComp ? Cast<UG2IPipesSplineMetadata>(SplineComp->GetSplinePointsMetadata()) : nullptr;
	return Metadata;
}

void FG2IPipesSplineMetadataDetails::OnSetValues(FG2IPipesSplineMetadataDetails& Details)
{
	Details.SplineComp->GetSplinePointsMetadata()->Modify();
	Details.SplineComp->UpdateSpline();
	Details.SplineComp->bSplineHasBeenEdited = true;
	static FProperty* SplineCurvesProperty = FindFProperty<FProperty>(USplineComponent::StaticClass(), GET_MEMBER_NAME_CHECKED(USplineComponent, SplineCurves));
	FComponentVisualizer::NotifyPropertyModified(Details.SplineComp, SplineCurvesProperty);
	Details.Update(Details.SplineComp, Details.SelectedKeys);

	GEditor->RedrawLevelEditingViewports(true);
}

TOptional<bool> FG2IPipesSplineMetadataDetails::GetHasPipe() const
{
	return bHasPipeValue;
}

void FG2IPipesSplineMetadataDetails::OnSetHasPipe(ECheckBoxState CheckState)
{
	if (UG2IPipesSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetHasPipe", "Set spline point hasPipe bool"));

		for (int32 Index : SelectedKeys)
		{
			Metadata->PointParams[Index].bHasPipe = (bool)CheckState;
		}

		OnSetValues(*this);
	}
}

TOptional<float> FG2IPipesSplineMetadataDetails::GetTestFloat() const
{
	return TestFloatValue;
}

void FG2IPipesSplineMetadataDetails::OnSetTestFloat(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UG2IPipesSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetTestFloat", "Set spline point test float"));

		for (int32 Index : SelectedKeys)
		{
			Metadata->PointParams[Index].TestValue = NewValue;
		}

		OnSetValues(*this);
	}
}
