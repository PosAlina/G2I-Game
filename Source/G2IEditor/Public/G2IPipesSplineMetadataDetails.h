#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "SplineMetadataDetailsFactory.h"
#include "G2IPipe.h"
#include "SplinesMetadata/G2IPipesSplineMetadata.h"
#include "G2IPipesSplineMetadataDetails.generated.h"

class UG2IPipesSplineMetadataDetails;
class FG2IPipesSplineMetadataDetails;

UCLASS()
class G2IEDITOR_API UG2IPipesSplineMetadataDetails : public USplineMetadataDetailsFactoryBase
{
	GENERATED_BODY()
	
	virtual ~UG2IPipesSplineMetadataDetails() {}
	virtual TSharedPtr<ISplineMetadataDetails> Create() override;
	virtual UClass* GetMetadataClass() const override;
};

class G2IEDITOR_API FG2IPipesSplineMetadataDetails : public ISplineMetadataDetails, public TSharedFromThis<FG2IPipesSplineMetadataDetails>
{
public:

	virtual ~FG2IPipesSplineMetadataDetails() {}
	virtual FName GetName() const override;
	virtual FText GetDisplayName() const override;
	virtual void Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys) override;
	virtual void GenerateChildContent(IDetailGroup& DetailGroup) override;

private:
	UG2IPipesSplineMetadata* GetMetadata() const;
	void OnSetValues(FG2IPipesSplineMetadataDetails& Details);
	TOptional<bool> GetHasPipe() const;
	void OnSetHasPipe(ECheckBoxState CheckState);
	TOptional<float> GetTestFloat() const;
	void OnSetTestFloat(float NewValue, ETextCommit::Type CommitInfo);

	TOptional<float> TestFloatValue;
	TOptional<bool> bHasPipeValue;
	//TOptional<bool> bHasValveValue;
	//TOptional<bool> bHasTechnicalHoleValue;
	//TOptional<bool> bSendToOtherPipeValue;
	//TOptional<bool> bResieveFromOtherPipeValue;
	USplineComponent* SplineComp = nullptr;
	TSet<int32> SelectedKeys;
};