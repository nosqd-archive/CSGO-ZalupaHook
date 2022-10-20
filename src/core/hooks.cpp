#include "hooks.h"

// include minhook for epic hookage
#include "../../ext/minhook/minhook.h"

#include <intrin.h>

#include "../hacks/misc.h"

void hooks::Setup() noexcept
{
	MH_Initialize();

	// AllocKeyValuesMemory hook
	MH_CreateHook(
		memory::Get(interfaces::keyValuesSystem, 1),
		&AllocKeyValuesMemory,
		reinterpret_cast<void**>(&AllocKeyValuesMemoryOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::clientMode, 44),
		&DoPostScreenSpaceEffects,
		reinterpret_cast<void**>(&DoPostScreenSpaceEffectsOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::clientMode, 24),
		&CreateMove,
		reinterpret_cast<void**>(&CreateMoveOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::studioRender, 29),
		&DrawModel,
		reinterpret_cast<void**>(&DrawModelOriginal)
	);

	MH_EnableHook(MH_ALL_HOOKS);
}

void hooks::Destroy() noexcept
{
	// restore hooks
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	// uninit minhook
	MH_Uninitialize();
}

void* __stdcall hooks::AllocKeyValuesMemory(const std::int32_t size) noexcept
{
	// if function is returning to speficied addresses, return nullptr to "bypass"
	if (const std::uint32_t address = reinterpret_cast<std::uint32_t>(_ReturnAddress());
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesEngine) ||
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesClient)) 
		return nullptr;

	// return original
	return AllocKeyValuesMemoryOriginal(interfaces::keyValuesSystem, size);
}

bool __stdcall hooks::CreateMove(float frameTime, CUserCmd* cmd) noexcept
{
	// make sure this function is being called from CInput::CreateMove
	if (!cmd->commandNumber)
		return CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);

	// this would be done anyway by returning true
	if (CreateMoveOriginal(interfaces::clientMode, frameTime, cmd))
		interfaces::engine->SetViewAngles(cmd->viewAngles);

	// get our local player here
	globals::UpdateLocalPlayer();

	if (globals::localPlayer && globals::localPlayer->IsAlive())
	{
		// example bhop
		if (globals::bhop)
			hacks::RunBunnyHop(cmd);
	}

	return false;
}

void __stdcall hooks::DoPostScreenSpaceEffects(const void* viewSetup) noexcept {
	if (globals::localPlayer && interfaces::engine->IsInGame()) {
		for (int i = 0; i < interfaces::glow->glowObjects.size; i++)
		{
			IGlowManager::CGlowObject& glowObject = interfaces::glow->glowObjects[i];
			
			if (glowObject.IsUnused())
				continue;

			if (!glowObject.entity)
				continue;

			switch (glowObject.entity->GetClientClass()->classID)
			{
			case CClientClass::CCSPlayer:
				if (!glowObject.entity->IsAlive())
					break;

				if (glowObject.entity->GetTeam() != globals::localPlayer->GetTeam() && globals::glowEnemy) {
					glowObject.SetColor(globals::glowEnemyCol[0], globals::glowEnemyCol[1], globals::glowEnemyCol[2]);
				}
				if (glowObject.entity->GetTeam() == globals::localPlayer->GetTeam() && globals::glowMate) {
					glowObject.SetColor(globals::glowMateCol[0], globals::glowMateCol[1], globals::glowMateCol[2]);

	
				}
				break;
			default:
				break;
			}
		}
	}

	return DoPostScreenSpaceEffectsOriginal(interfaces::clientMode, viewSetup);
}

void __stdcall hooks::DrawModel(
	void* results,
	const CDrawModelInfo& info,
	CMatrix3x4* bones,
	float* flexWeights,
	float* flexDelayedWeights,
	const CVector& modelOrigin,
	const std::int32_t flags) noexcept {
	if (globals::localPlayer && info.renderable && interfaces::studioRender->IsForcedMaterialOverride() == false) {
		CEntity* ent = info.renderable->GetIClientUnknown()->GetBaseEntity();

		if (ent && ent->IsPlayer() && ent->GetTeam() != globals::localPlayer->GetTeam() && (globals::visualsChamsHidden || globals::visualsChamsVisible)) {
			static IMaterial* mat = interfaces::materialSystem->FindMaterial("debug/debugambientcube");

			interfaces::studioRender->SetAlphaModulation(1.f);
			
			if (globals::visualsChamsHidden) {
				mat->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
				interfaces::studioRender->SetColorModulation(globals::visualsChamsHiddenCol);
				interfaces::studioRender->ForcedMaterialOverride(mat);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
			}
			
			if (globals::visualsChamsVisible) {
				mat->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
				interfaces::studioRender->SetColorModulation(globals::visualsChamsVisibleCol);
				interfaces::studioRender->ForcedMaterialOverride(mat);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
			}

			return interfaces::studioRender->ForcedMaterialOverride(nullptr);
		}
	}
	DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);


}
