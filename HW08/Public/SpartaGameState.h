#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

UCLASS()
class HW08_API ASpartaGameState : public AGameState
{
    GENERATED_BODY()

public:
    ASpartaGameState();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    float LevelDuration;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MaxWaves; 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    float WaveDuration; 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    FTimerHandle LevelTimerHandle; 
    FTimerHandle WaveTimerHandle;
    FTimerHandle HUDUpdateTimerHandle;

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();

    void StartLevel();
    void StartWave();
    void OnWaveTimeUp(); 
    void NextWave(); 
    void OnCoinCollected();
    void EndLevel();
    void UpdateHUD();
};