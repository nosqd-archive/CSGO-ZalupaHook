#include "Gui.h"
#include "./src/core/globals.h"

// Create the type of function that we will hook
typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;

typedef long(__stdcall* Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
static Reset oReset = NULL;

// Declare the detour function

static WNDPROC oWndProc = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hkWindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (globals::menu)
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0)
			return 1L;
	return ::CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}

long __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	long result = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	static bool init = false;

	if (!init)
	{
		D3DDEVICE_CREATION_PARAMETERS params;
		pDevice->GetCreationParameters(&params);

		oWndProc = (WNDPROC)::SetWindowLongPtr((HWND)params.hFocusWindow, GWLP_WNDPROC, (LONG)hkWindowProc);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(params.hFocusWindow);
		ImGui_ImplDX9_Init(pDevice);

		init = true;
		globals::menu = true;
	}

	if (globals::menu) {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		gui::Render();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	

	return oEndScene(pDevice);
}

void gui::Setup() {

		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{

			kiero::bind(42, (void**)&oEndScene, hkEndScene);
			kiero::bind(16, (void**)&oReset, hkReset);
		}

	MessageBox(0, "Hello", "Hello", MB_OK);

}

void gui::Destroy() {
	kiero::shutdown();
}

void gui::Render() {
	ImGui::Begin("zalupahook dev");
	ImGui::Checkbox("Bunnyhop", &globals::bhop);

	ImGui::Checkbox("Chams Visible##1", &globals::visualsChamsVisible);
	ImGui::ColorEdit3("Chams Visible##2", globals::visualsChamsVisibleCol);

	ImGui::Checkbox("Chams Hidden##1", &globals::visualsChamsHidden);
	ImGui::ColorEdit3("Chams Hidden##2", globals::visualsChamsHiddenCol);

	ImGui::Checkbox("Glow Enemy##1", &globals::glowEnemy);
	ImGui::ColorEdit3("Glow Enemy##2", globals::glowEnemyCol);

	ImGui::Checkbox("Glow Mates##1", &globals::glowMate);
	ImGui::ColorEdit3("Glow Mates##2", globals::glowMateCol);

	ImGui::End();
}