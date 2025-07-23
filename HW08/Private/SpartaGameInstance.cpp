#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void USpartaGameInstance::AddToScore(int32 Amount)
{
    if (Amount >= 0)
    {
        TotalScore += Amount;
        UE_LOG(LogTemp, Warning, TEXT("Total Score Updated: %d"), TotalScore);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Amount (%d) for AddToScore!"), Amount);
    }
}