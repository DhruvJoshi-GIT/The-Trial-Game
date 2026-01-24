// Neon Ascension - Game Mode Implementation

#include "GameModes/NeonGameMode.h"

ANeonGameMode::ANeonGameMode()
{
	// Set default pawn class (will be set in Blueprint)
}

void ANeonGameMode::StartWave(int32 WaveNumber)
{
	CurrentWave = WaveNumber;

	// Calculate enemies based on wave and difficulty
	int32 BaseEnemies = 3 + WaveNumber * 2;
	EnemiesRemaining = BaseEnemies * CurrentDifficulty;

	OnWaveStarted(WaveNumber);
}

void ANeonGameMode::OnEnemyDefeated()
{
	EnemiesRemaining--;

	if (EnemiesRemaining <= 0)
	{
		OnWaveCompleted(CurrentWave);

		if (CurrentWave >= TotalWaves)
		{
			OnArenaCompleted();
		}
	}
}

void ANeonGameMode::OnPlayerDeath()
{
	OnGameOver();
}
