// Neon Ascension - Game Mode

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NeonGameMode.generated.h"

UCLASS()
class NEONASCENSION_API ANeonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANeonGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 CurrentWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 TotalWaves = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 CurrentDifficulty = 1;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartWave(int32 WaveNumber);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnEnemyDefeated();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPlayerDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnWaveStarted(int32 WaveNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnWaveCompleted(int32 WaveNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnArenaCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnGameOver();

protected:
	int32 EnemiesRemaining = 0;
};
