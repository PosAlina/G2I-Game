


#include "AnimNotify/G2IInputControlNotify.h"

#include "G2I.h"
#include "Components/G2IGlovePunchComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UG2IInputControlNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogG2I, Warning, TEXT("Owner is null"));
		return;
	}

	UG2IGlovePunchComponent* GlovePunchComp = Owner->FindComponentByClass<UG2IGlovePunchComponent>();
	if (!GlovePunchComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("GlovePunchComp is null"));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character){
		UE_LOG(LogG2I, Warning, TEXT("Owner is not a Character"));
		return;
	}
	
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC)
	{
		UE_LOG(LogG2I, Warning, TEXT("PC is null"));
		return;
	}

	if (GlovePunchComp->bIsNeedToControlInput)
	{
		bEnableInput ? PC->EnableInput(PC) : PC->DisableInput(PC);
	}
}
