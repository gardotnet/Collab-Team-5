#include "BaseTrap.h"

ABaseTrap::ABaseTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
}

bool ABaseTrap::ActivateTrap()
{
	if (IsTrapOn)
		return false;
	
	IsTrapOn = true;
	if (IsDelayed)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseTrap::StartTrapAnimation, DelayTime, false);
	}
	else
	{
		StartTrapAnimation();
	}
	return true;
}

void ABaseTrap::StartTrapAnimation()
{
	UE_LOG(LogTemp, Warning, TEXT("Pretend I have trap animations :P"));
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseTrap::DisableTrap, TrapDisableTime, false);
}

void ABaseTrap::DisableTrap()
{
	IsTrapOn = false;
}

