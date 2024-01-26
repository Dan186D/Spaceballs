#include "imgui_impl_raylib.h"
#include <raylib.h>
#include <rlgl.h>
#include <memory>

static double g_Time = 0.0;
static bool g_UnloadAtlas = false;
static int g_AtlasTexID = 0;

static const char* ImGui_ImplRaylib_GetClipboardText(void*)
{
    return GetClipboardText();
}

static void ImGui_ImplRaylib_SetClipboardText(void*, const char* text)
{
    SetClipboardText(text);
}

bool ImGui_ImplRaylib_Init()
{
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = "imgui_impl_raylib";

    io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_Home] = KEY_HOME;
    io.KeyMap[ImGuiKey_End] = KEY_END;
    io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = KEY_A;
    io.KeyMap[ImGuiKey_C] = KEY_C;
    io.KeyMap[ImGuiKey_V] = KEY_V;
    io.KeyMap[ImGuiKey_X] = KEY_X;
    io.KeyMap[ImGuiKey_Y] = KEY_Y;
    io.KeyMap[ImGuiKey_Z] = KEY_Z;

    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    io.SetClipboardTextFn = ImGui_ImplRaylib_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplRaylib_GetClipboardText;
    io.ClipboardUserData = NULL;

#ifdef AUTO_FONTATLAS
    ImGui_ImplRaylib_LoadDefaultFontAtlas();
#endif

    return true;
}

void ImGui_ImplRaylib_Shutdown()
{
    if (g_UnloadAtlas) {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->ClearTexData();
    }
    g_Time = 0.0;
}

static void ImGui_ImplRaylib_UpdateMousePosAndButtons()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
        SetMousePosition(io.MousePos.x, io.MousePos.y);
    else
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    io.MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

    if (!IsWindowMinimized())
        io.MousePos = ImVec2(GetMouseX(), GetMouseY());
}

static void ImGui_ImplRaylib_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        HideCursor();
    }
    else
    {
        // Show OS mouse cursor
        ShowCursor();
    }
}

void ImGui_ImplRaylib_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)GetScreenWidth(), (float)GetScreenHeight());

    double current_time = GetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    io.KeyCtrl = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    io.KeyShift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
    io.KeyAlt = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
    io.KeySuper = IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);

    ImGui_ImplRaylib_UpdateMousePosAndButtons();
    ImGui_ImplRaylib_UpdateMouseCursor();

    if (GetMouseWheelMove() > 0)
        io.MouseWheel += 1;
    else if (GetMouseWheelMove() < 0)
        io.MouseWheel -= 1;
}

#define FOR_ALL_KEYS(X) \
    X(KEY_APOSTROPHE); \
    X(KEY_COMMA); \
    X(KEY_MINUS); \
    X(KEY_PERIOD); \
    X(KEY_SLASH); \
    X(KEY_ZERO); \
    X(KEY_ONE); \
    X(KEY_TWO); \
    X(KEY_THREE); \
    X(KEY_FOUR); \
    X(KEY_FIVE); \
    X(KEY_SIX); \
    X(KEY_SEVEN); \
    X(KEY_EIGHT); \
    X(KEY_NINE); \
    X(KEY_SEMICOLON); \
    X(KEY_EQUAL); \
    X(KEY_A); \
    X(KEY_B); \
    X(KEY_C); \
    X(KEY_D); \
    X(KEY_E); \
    X(KEY_F); \
    X(KEY_G); \
    X(KEY_H); \
    X(KEY_I); \
    X(KEY_J); \
    X(KEY_K); \
    X(KEY_L); \
    X(KEY_M); \
    X(KEY_N); \
    X(KEY_O); \
    X(KEY_P); \
    X(KEY_Q); \
    X(KEY_R); \
    X(KEY_S); \
    X(KEY_T); \
    X(KEY_U); \
    X(KEY_V); \
    X(KEY_W); \
    X(KEY_X); \
    X(KEY_Y); \
    X(KEY_Z); \
    X(KEY_SPACE); \
    X(KEY_ESCAPE); \
    X(KEY_ENTER); \
    X(KEY_TAB); \
    X(KEY_BACKSPACE); \
    X(KEY_INSERT); \
    X(KEY_DELETE); \
    X(KEY_RIGHT); \
    X(KEY_LEFT); \
    X(KEY_DOWN); \
    X(KEY_UP); \
    X(KEY_PAGE_UP); \
    X(KEY_PAGE_DOWN); \
    X(KEY_HOME); \
    X(KEY_END); \
    X(KEY_CAPS_LOCK); \
    X(KEY_SCROLL_LOCK); \
    X(KEY_NUM_LOCK); \
    X(KEY_PRINT_SCREEN); \
    X(KEY_PAUSE); \
    X(KEY_F1); \
    X(KEY_F2); \
    X(KEY_F3); \
    X(KEY_F4); \
    X(KEY_F5); \
    X(KEY_F6); \
    X(KEY_F7); \
    X(KEY_F8); \
    X(KEY_F9); \
    X(KEY_F10); \
    X(KEY_F11); \
    X(KEY_F12); \
    X(KEY_LEFT_SHIFT); \
    X(KEY_LEFT_CONTROL); \
    X(KEY_LEFT_ALT); \
    X(KEY_LEFT_SUPER); \
    X(KEY_RIGHT_SHIFT); \
    X(KEY_RIGHT_CONTROL); \
    X(KEY_RIGHT_ALT); \
    X(KEY_RIGHT_SUPER); \
    X(KEY_KB_MENU); \
    X(KEY_LEFT_BRACKET); \
    X(KEY_BACKSLASH); \
    X(KEY_RIGHT_BRACKET); \
    X(KEY_GRAVE); \
    X(KEY_KP_0); \
    X(KEY_KP_1); \
    X(KEY_KP_2); \
    X(KEY_KP_3); \
    X(KEY_KP_4); \
    X(KEY_KP_5); \
    X(KEY_KP_6); \
    X(KEY_KP_7); \
    X(KEY_KP_8); \
    X(KEY_KP_9); \
    X(KEY_KP_DECIMAL); \
    X(KEY_KP_DIVIDE); \
    X(KEY_KP_MULTIPLY); \
    X(KEY_KP_SUBTRACT); \
    X(KEY_KP_ADD); \
    X(KEY_KP_ENTER); \
    X(KEY_KP_EQUAL);

#define SET_KEY_DOWN(KEY) io.KeysDown[KEY] = IsKeyDown(KEY)

bool ImGui_ImplRaylib_ProcessEvent()
{
    ImGuiIO& io = ImGui::GetIO();

    FOR_ALL_KEYS(SET_KEY_DOWN);

    // Uncomment the three lines below if using raylib earlier than version 3.
    //if (GetKeyPressed() != -1)
    //{
#ifdef ENABLE_SCODETOUTF8
    int length;     //  Length was only ever created to be passed to CodepointToUtf8(), since it doesn't check for nullptrs.
    io.AddInputCharactersUTF8(CodepointToUTF8(GetCharPressed(), &length));
    (void)length;   //  Silencing the compiler warnings.
#else
    io.AddInputCharacter(GetKeyPressed());
#endif
    //}

    return true;
}

#ifdef COMPATIBILITY_MODE
void ImGui_ImplRaylib_LoadDefaultFontAtlas()
{
    if (!g_UnloadAtlas) {
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels = NULL;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL);
        Image image = GenImageColor(width, height, BLANK);
        memcpy(image.data, pixels, width*height*4);

        Texture2D* tex = (Texture2D*)io.Fonts->TexID;
				if (tex && tex->id != 0) {
					UnloadTexture(*tex);
					MemFree(tex);
				}
				tex = (Texture2D*)MemAlloc(sizeof(Texture2D));
				*tex = LoadTextureFromImage(image);
				UnloadImage(image);
				g_AtlasTexID = tex->id;
				io.Fonts->TexID = tex;

        g_UnloadAtlas = true;
    }
};

static void ImGuiTriangleVert(ImDrawVert& idx_vert)
{
	Color* c;
	c = (Color*)&idx_vert.col;
	rlColor4ub(c->r, c->g, c->b, c->a);
	rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
	rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
}

static void ImGuiRenderTriangles(unsigned int count, int indexStart, const ImVector<ImDrawIdx>& indexBuffer, const ImVector<ImDrawVert>& vertBuffer, void* texturePtr)
{
	if (count < 3)
		return;

	Texture* texture = (Texture*)texturePtr;

	unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

	rlBegin(RL_TRIANGLES);
	rlSetTexture(textureId);

	for (unsigned int i = 0; i <= (count - 3); i += 3)
	{
		if (rlCheckRenderBatchLimit(3))
		{
			rlBegin(RL_TRIANGLES);
			rlSetTexture(textureId);
		}

		ImDrawIdx indexA = indexBuffer[indexStart + i];
		ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
		ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

		ImDrawVert vertexA = vertBuffer[indexA];
		ImDrawVert vertexB = vertBuffer[indexB];
		ImDrawVert vertexC = vertBuffer[indexC];

		ImGuiTriangleVert(vertexA);
		ImGuiTriangleVert(vertexB);
		ImGuiTriangleVert(vertexC);
	}
	rlEnd();
}

static void EnableScissor(float x, float y, float width, float height)
{
	rlEnableScissorTest();
	ImGuiIO& io = ImGui::GetIO();
	rlScissor((int)(x * io.DisplayFramebufferScale.x),
		int((GetScreenHeight() - (int)(y + height)) * io.DisplayFramebufferScale.y),
		(int)(width * io.DisplayFramebufferScale.x),
		(int)(height * io.DisplayFramebufferScale.y));
}

//  Code originally provided by WEREMSOFT.
void ImGui_ImplRaylib_Render(ImDrawData* draw_data)
{
	rlDrawRenderBatchActive();
	rlDisableBackfaceCulling();

	for (int l = 0; l < draw_data->CmdListsCount; ++l)
	{
		const ImDrawList* commandList = draw_data->CmdLists[l];

		for (const auto& cmd : commandList->CmdBuffer)
		{
			EnableScissor(cmd.ClipRect.x - draw_data->DisplayPos.x, cmd.ClipRect.y - draw_data->DisplayPos.y, cmd.ClipRect.z - (cmd.ClipRect.x - draw_data->DisplayPos.x), cmd.ClipRect.w - (cmd.ClipRect.y - draw_data->DisplayPos.y));
			if (cmd.UserCallback != nullptr)
			{
				cmd.UserCallback(commandList, &cmd);

				continue;
			}

			ImGuiRenderTriangles(cmd.ElemCount, cmd.IdxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
			rlDrawRenderBatchActive();
		}
	}

	rlSetTexture(0);
	rlDisableScissorTest();
	rlEnableBackfaceCulling();
}
#endif