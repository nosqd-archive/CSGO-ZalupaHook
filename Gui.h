#include <d3d9.h>
#include <Windows.h>
#include "./ext/imgui/imgui_impl_win32.h"
#include "./ext/imgui/imgui_impl_dx9.h"
#include "./ext/imgui/imgui.h"
#include "ext/kiero/kiero.h"

namespace gui {
	void Setup();
	void Destroy();
	void Render();
}