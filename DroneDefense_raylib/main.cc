
#include "pch.h"
#include "LevelFile.h"
#include "common.h"
#include "imgui-impl-raylib/imgui_impl_raylib.h"
#include "imgui-impl-raylib/imgui_impl_raylib_config.h"
#include "imgui-1.73/imgui.h"
#include "imgui-1.73/imconfig.h"
#include "config.h"

level_t level;
int killed_enemies = 0;
float starting_widget_len = 10.0f;
float starting_widget_width = 5.0f;
WIDGET current_widget = WIDGET_TRANSLATE;
bool mult_select = false;
unsigned int selected_light = 0;
bool imgui_hovered = false;
Shader ground_shader;
Model skybox;

const float cam_far = RL_CULL_DISTANCE_FAR;
const float cam_near = RL_CULL_DISTANCE_NEAR;
#define UNIFORMS \
X(ground_shader, time, &game_time_s, SHADER_UNIFORM_FLOAT)\

#define X(shader, name, address, type) unsigned int u_ ## name ## _loc;
UNIFORMS
#undef X

//Editor
bool editor_mode = false;
bool debug_collision_detection = false;
bool debug_enemy = false;

bool paused = false;
float game_timescale = 1.0f;
float game_time_s = 0.0f;

bool extrap = true;

unsigned int screen_width = int(1920*1.7);
unsigned int screen_height = int(1080*1.7);
bool fullscreen = false;

input_state_t inputs;

//@HACK
bool healed = false;

void reset() {
}

#define INPUT(KEY_FLAG) \
if(IsKeyDown(KEY_ ## KEY_FLAG)){inputs.input_state |= INPUT_FLAGS::KEY_FLAG;}

void input() {
	inputs.prev_input_state = inputs.input_state;
	inputs.input_state = 0;

	ImGui_ImplRaylib_ProcessEvent();

	if(IsWindowResized()) {
		screen_width = GetScreenWidth();
		screen_height = GetScreenHeight();
	}

	if(IsWindowFocused() && !imgui_hovered) {

		INPUT(W)
		INPUT(S)
		INPUT(A)
		INPUT(D)
		INPUT(Q)
		INPUT(E)
		INPUT(T)
		INPUT(Z)
		INPUT(R)
		INPUT(SPACE)
		INPUT(LEFT_SHIFT)
		INPUT(ENTER)
		INPUT(RIGHT)
		INPUT(LEFT)
		INPUT(P)

		if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
			inputs.input_state |= INPUT_FLAGS::LMB;

		if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
			inputs.input_state |= INPUT_FLAGS::RMB;

		if(IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
			inputs.input_state |= INPUT_FLAGS::MMB;

		inputs.generate_input_events();

		// get the mouse position
		Vector2 mouse_pos_screen = GetMousePosition();

		inputs.mouse_pos = screen_coords_to_ndc(mouse_pos_screen, Vector2{ (float)screen_width, (float)screen_height });

		if(mouse_pos_screen == Vector2{ (float)(screen_width/2), (float)(screen_height/2) }) { inputs.mouse_pos = Vector2Zero(); };

		bool move_camera = editor_mode ? inputs.input_state & INPUT_FLAGS::RMB && !(inputs.input_down & INPUT_FLAGS::RMB) : true;

		bool set_mouse_pos = editor_mode ? inputs.input_state & INPUT_FLAGS::RMB : true;

		// Set look position
		if(move_camera) {
		}

		if(set_mouse_pos) {
			SetMousePosition(screen_width/2, screen_height/2);
		}
	}
}

void editor_update() {
	if (inputs.input_down & INPUT_FLAGS::ENTER) {
		editor_mode = !editor_mode;
		if (editor_mode) {
			ShowCursor();
		}
		HideCursor();
	}
}

void update() {
	static timer_t t;
	t.freq_hz = 1.0f/1.0f;
	bool activate = t.tick(dt_s);
	if(activate) {
		//particle_system.activate_particles(10, level.startPos, UNIT_VECS[DIR_Y], RED, 5.0f);
		t.reset();
	}

	if(inputs.input_state & INPUT_FLAGS::EXIT) {
		CloseWindow();
	}

	if(inputs.input_down & INPUT_FLAGS::R) {
		reset();
	}
	
}

void imgui() {
	char* listbox_display[500] = {};

	if(editor_mode) {
		ImGui_ImplRaylib_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowBgAlpha(0.1f);
		ImGui::Begin("Editor", 0, ImGuiWindowFlags_NoTitleBar);

		ImGuiIO& IMGUIio = ImGui::GetIO();
		imgui_hovered = IMGUIio.WantCaptureMouse;
		ImGui::ShowDemoWindow();

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if(ImGui::BeginTabBar("Editor", tab_bar_flags) && editor_mode)
		{
			if(ImGui::BeginTabItem("Options/Perf")) {
#if 0
				static int resIndex = 0;
				const unsigned int res[][2] = { {1920, 1080}, {1680, 1050}, {1600, 1024}, {1600, 900},
					{1366, 768}, {1280, 1024}, {1280, 960}, {1280, 720}, {800, 600}, {640, 480} };
				if(ImGui::Combo("Resolution", &resIndex, " 1920x1080 \0 1680x1050 \0 1600x1024 \0 1600x900 \0 1366x768 \0 1280x1024 \0 1280x960 \0 1280x720 \0 800x600 \0 640x480 \0\0"))
				{
					screenWidth = res[resIndex][0];
					screenHeight = res[resIndex][1];
					recreateWindow();
				}
				if(ImGui::Checkbox("Fullscreen", &fullscreen)) {
					recreateWindow();
				}
				static int aliasIndex = 4;
				const unsigned int alias[] = { 0, 2, 4, 6, 8 };
				if(ImGui::Combo("Antialiasing", &aliasIndex, " off \0 2 \0 4 \0 6 \0 8 \0\0")) {
					settings.antialiasingLevel = alias[aliasIndex];
					recreateWindow();
				}
#endif
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();

			ImGui::End();

		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplRaylib_Render(ImGui::GetDrawData());
	}
}

void draw(float extrap_dt_s, float game_dt) {
	BeginDrawing();

	Color clear = vec3_to_color(level.clearColour);

	ClearBackground(clear);

	//BeginMode2D();

#define X(shader, name, address, type)\
SetShaderValue(shader, u_ ## name ## _loc, address, type);
	UNIFORMS
#undef X

	EndMode2D();

	DrawFPS(10, 10);

	imgui();

	EndDrawing();
}

void load_shaders_and_font() {
	ground_shader = LoadShader(shader_dir "base_lighting.vs", shader_dir "shader.fs");

	ground_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(ground_shader, "matModel");

#define X(shader, name, address, type)\
u_ ## name ## _loc = GetShaderLocation(shader, "u_" #name "\0" );
	UNIFORMS
#undef X

}

int main() {
	ImGuiContext* c = ImGui::CreateContext();
	ImGui::SetCurrentContext(c);
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	InitWindow(screen_width, screen_height, "Censer");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	ImGui_ImplRaylib_Init();
	editor_mode = true;

	load_shaders_and_font();

	reset();

	//SetCameraMode(player_camera, CAMERA_PERSPECTIVE);

	SetTargetFPS(120);

	static float lag_s = 0.0f;

	HideCursor();
	while(!WindowShouldClose())
	{
		const float dt = GetFrameTime();

		float game_dt = paused ? 0.0f : dt*game_timescale;

		game_time_s += game_dt;

		lag_s += game_dt;

		//if a frame took more than a second we are probably at a break
		//or something is really wrong
#if _DEBUG
		lag_s = fmin(1.0f, lag_s);
#endif

		int updates = (int)(lag_s/dt_s);
		input();

		if(inputs.input_down & INPUT_FLAGS::P) {
			paused = !paused;
			inputs.input_down ^= INPUT_FLAGS::P;
		}

		for(int i = 0; i < updates; i++) {
			if (i == updates - 1) {
				//printf("hello\n");
			}
			update();
#if _DEBUG
			editor_update();
#endif
			inputs.consume_input();
		}
		lag_s = fmodf(lag_s, dt_s);

		assert(lag_s < dt_s && lag_s >= 0.0f);

		float extrap_s = extrap ? lag_s : 0.0f;
		draw(extrap_s, game_dt);
		//std::this_thread::sleep_for(chrono::milliseconds(400));
	}
	CloseWindow();        // Close window and OpenGL context

	return 0;
}

