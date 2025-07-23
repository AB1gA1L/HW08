#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "Engine/Engine.h" 
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpartaCharacter.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 3;

    CurrentWaveIndex = 0;
    MaxWaves = 3;
    WaveDuration = 30.0f;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    USpartaGameInstance* GameInstance = Cast<USpartaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        CurrentLevelIndex = GameInstance->CurrentLevelIndex;
        UE_LOG(LogTemp, Log, TEXT("Initial CurrentLevelIndex: %d"), CurrentLevelIndex);
    }
    // 타이머 초기화
    GetWorldTimerManager().ClearAllTimersForObject(this); 
    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        1.0f,
        true
    );
    UpdateHUD();
    StartLevel();
}

void ASpartaGameState::StartWave()
{
    GetWorldTimerManager().ClearAllTimersForObject(this);
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    if (GEngine && CurrentWaveIndex == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Wave %d 시작!"), CurrentWaveIndex + 1));
    }
    UE_LOG(LogTemp, Warning, TEXT("Level %d, Wave %d 시작!"), CurrentLevelIndex + 1, CurrentWaveIndex + 1);

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemsToSpawn = 20 + (CurrentWaveIndex * 10);

    for (int32 i = 0; i < ItemsToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASpartaGameState::OnWaveTimeUp,
        WaveDuration,
        false
    );
    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        1.0f,
        true
    );
    UpdateHUD();
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
        if (SpartaPlayerController)
        {
            UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget();
            if (HUDWidget)
            {
                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            int32 CurrentScore = SpartaGameInstance->TotalScore;
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore)));
                            UE_LOG(LogTemp, Log, TEXT("Score updated to: %d"), CurrentScore);
                        }
                        else
                        {
                            ScoreText->SetText(FText::FromString(TEXT("Score: 0")));
                            UE_LOG(LogTemp, Warning, TEXT("SpartaGameInstance is null!"));
                        }
                    }
                }

                if (UTextBlock* HealthText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Health"))))
                {
                    if (APawn* ControlledPawn = PlayerController->GetPawn())
                    {
                        ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(ControlledPawn);
                        if (SpartaCharacter)
                        {
                            float Health = SpartaCharacter->GetHealth();
                            HealthText->SetText(FText::FromString(FString::Printf(TEXT("Health: %.0f"), FMath::Clamp(Health, 0.0f, 100.0f))));
                            UE_LOG(LogTemp, Log, TEXT("Health updated to: %.0f"), Health);
                        }
                    }
                }

                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
                    if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle) && RemainingTime >= 0.0f)
                    {
                        TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                    }
                    else
                    {
                        TimeText->SetText(FText::FromString(TEXT("Time: 0.0")));
                    }
                    UE_LOG(LogTemp, Log, TEXT("Time updated to: %.1f, Timer Active: %d"), RemainingTime, GetWorldTimerManager().IsTimerActive(WaveTimerHandle));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("HUDWidget is null in UpdateHUD!"));
            }
        }
    }
}