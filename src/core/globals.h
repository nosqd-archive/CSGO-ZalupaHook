#pragma once

class CEntity;
namespace globals
{
	inline CEntity* localPlayer = nullptr;
	inline bool menu = false;

	inline bool visualsChamsVisible = false;
	inline float visualsChamsVisibleCol[3] = {0.f, 1.f, 0.f};
	inline bool visualsChamsHidden = false;
	inline float visualsChamsHiddenCol[3] = { 0.f, 1.f, 0.f };

	inline bool glowEnemy = false;
	inline float glowEnemyCol[3] = { 0.f, 1.f, 0.f };

	inline bool glowMate = false;
	inline float glowMateCol[3] = { 0.f, 1.f, 0.f };

	inline bool bhop = false;


	// update the local player pointer
	void UpdateLocalPlayer() noexcept;
}
