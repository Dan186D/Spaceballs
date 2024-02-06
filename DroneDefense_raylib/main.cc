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
float G_constant = 2.0;
Camera2D camera = { 0 };

#define PREVIEW_NUM 50
Vector2 preview[PREVIEW_NUM];

struct body_t {
	Vector2 position;
	Vector2 velocity_mps;
	float radius_m;
	float mass_kg;

	// Shader locations
	unsigned int u_position_loc;
	unsigned int u_radius_loc;
	unsigned int u_mass_loc;
};

#define NUM_BODIES 5
body_t bodies[NUM_BODIES];

void update_bodies() {

}

void draw_bodies() {

	for (int i = 0; i < NUM_BODIES; i++) {
		const float bodyDensity = bodies[i].mass_kg / bodies[i].radius_m;
		const Color bodyColor = lerp(PINK, DARKBROWN, Clamp(bodyDensity / 10.0f, 0.0f, 1.0f));
		DrawCircle(bodies[i].position.x, bodies[i].position.y, bodies[i].radius_m, bodyColor);
	}
}

Vector2 calc_gravity(Vector2 position) {
		Vector2 gravity_force_N = Vector2Zero();

		for (int i = 0; i < NUM_BODIES; i++) {
			float dist_M = Vector2Distance(position, bodies[i].position);
			float gravity_strength_N = (G_constant * bodies[i].mass_kg) / pow(dist_M, 1.5f);
			Vector2 direction = Vector2Normalize(bodies[i].position - position);

			gravity_force_N += direction * gravity_strength_N;
		}

		return gravity_force_N;

}

struct ball_t {
	Vector2 position = Vector2Zero();
	Vector2 velocity_mps = Vector2Zero();
	float radius_m = 10;
	bool active = false;

	void init(Vector2 pos) {
		position = pos;
		active = true;
	}

	void init(Vector2 pos, Vector2 vel) {
		init(pos);
		velocity_mps = vel;
	}

	void update() {
		velocity_mps += calc_gravity(position);
		position += velocity_mps;

		for (int i = 0; i < NUM_BODIES; i++) {
			float combinedRadii = bodies[i].radius_m + radius_m;
			if (Vector2Distance(bodies[i].position, position) <= combinedRadii) {
				Vector2 normal = Vector2Normalize(position - bodies[i].position);
				position = bodies[i].position + normal * combinedRadii;
				velocity_mps = Vector2Reflect(velocity_mps, normal);
			}
		}
	}

	void draw() {
		DrawCircle(position.x, position.y, radius_m, RED);
	}

};

void calculate_preview(Vector2 start, Vector2 vel) {
	ball_t b;
	b.position = start;
	b.velocity_mps = vel;

	constexpr int n = 20;

	for (int i = 0; i < PREVIEW_NUM; i++) {
		for (int j = 0; j < n; j++) {
			b.update();
		}
		preview[i] = b.position;
	}
}
void draw_preview() {
	for (int i = 0; i < PREVIEW_NUM; i++) {
		DrawCircle(preview[i].x, preview[i].y, 10, GREEN);
	}
}

#define BALL_NUM 50
ball_t balls[BALL_NUM];

void update_balls() {
	for (int i = 0; i < BALL_NUM; i++) {
		if (balls[i].active) {
			balls[i].update();
		}
	}
}

void draw_balls() {
	for (int i = 0; i < BALL_NUM; i++) {
		if (balls[i].active) {
			balls[i].draw();
		}
	}
}

void add_ball(Vector2 pos, Vector2 vel) {
	static int ball_num = 0;
	balls[ball_num].init(pos, vel);

	ball_num = (ball_num + 1) % BALL_NUM;
}

void add_ball(Vector2 pos) {
	add_ball(pos, Vector2{ 0.0f, 0.0f });
}


struct player_t {
	Vector2 position = Vector2Zero();
	Vector2 velocity_mps = Vector2Zero();

	void update() {
		velocity_mps += calc_gravity(position);

		position += velocity_mps;
	}

	void draw() {
		DrawCircle(position.x, position.y, 20, BLUE);

	}
};

player_t player;

const float cam_far = RL_CULL_DISTANCE_FAR;
const float cam_near = RL_CULL_DISTANCE_NEAR;
#define UNIFORMS \
X(ground_shader, time, &game_time_s, SHADER_UNIFORM_FLOAT)\
X(ground_shader, screen_dims, &screen_dims, SHADER_UNIFORM_VEC2)\

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

Vector2 screen_dims = Vector2{ 1920.0f, 1080.0f };
bool fullscreen = false;

input_state_t inputs;
#define WAS_MOUSE_PRESSED inputs.prev_input_state & INPUT_FLAGS::LMB
#define IS_MOUSE_PRESSED inputs.input_state & INPUT_FLAGS::LMB

void reset() {
	camera.target = Vector2{ 0.0f, 0.0f };
	camera.offset = Vector2{ 0.0f, 0.0f };
	//camera.offset = Vector2{ 0.0f, 0.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	player.position = Vector2Zero();
	player.velocity_mps = Vector2Zero();

	screen_dims = Vector2{ (float)GetMonitorWidth(0), (float)GetMonitorWidth(0) } * 1.0f;
	SetWindowSize(screen_dims.x, screen_dims.y);
	SetWindowPosition(0, 0);

	for (int i = 0; i < NUM_BODIES; i++) {
		bodies[i].position = Vector2{ rand_norm() * screen_dims.x, rand_norm() * screen_dims.y};
		bodies[i].mass_kg = fmax(rand_norm() * rand_norm() * 1000.0f, 100);
		bodies[i].radius_m = fmax(rand_norm() * rand_norm() * 300.0f, 100);
	}
}

#define INPUT(KEY_FLAG) \
if(IsKeyDown(KEY_ ## KEY_FLAG)){inputs.input_state |= INPUT_FLAGS::KEY_FLAG;}

void input() {
	inputs.prev_input_state = inputs.input_state;
	inputs.input_state = 0;

	ImGui_ImplRaylib_ProcessEvent();

	if(IsWindowResized()) {
		screen_dims = Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() };
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

		inputs.mouse_pos = screen_coords_to_ndc(mouse_pos_screen, screen_dims);

		if(mouse_pos_screen == screen_dims * 0.5f) { inputs.mouse_pos = Vector2Zero(); };

		bool move_camera = editor_mode ? inputs.input_state & INPUT_FLAGS::RMB && !(inputs.input_down & INPUT_FLAGS::RMB) : true;

		bool set_mouse_pos = editor_mode ? inputs.input_state & INPUT_FLAGS::RMB : true;

		// Set look position
		if(move_camera) {
		}

		if(set_mouse_pos) {
			Vector2 halfScreen = screen_dims * 0.5f;
			SetMousePosition(halfScreen.x, halfScreen.y);
		}
	}
}

void editor_update() {
	if (inputs.input_down & INPUT_FLAGS::ENTER) {
		editor_mode = !editor_mode;
		if (editor_mode) {
			ShowCursor();
		}
		//HideCursor();
	}
}

Vector2 BallSpawnPosition{ 0.0f,0.0f };
Vector4 DebugLine{ 0.0f, 0.0f, 0.0f, 0.0f };

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

	DebugLine = Vector4{};
	if (inputs.input_up & INPUT_FLAGS::LMB) {
		add_ball(BallSpawnPosition, (BallSpawnPosition - GetMousePosition()) * 0.01f);
	}
	else if (IS_MOUSE_PRESSED && WAS_MOUSE_PRESSED) {
		DebugLine = { BallSpawnPosition.x, BallSpawnPosition.y, GetMousePosition().x, GetMousePosition().y };
		calculate_preview(BallSpawnPosition, (BallSpawnPosition - GetMousePosition()) * 0.01f);
	}
	else if (inputs.input_down & INPUT_FLAGS::LMB) {
		BallSpawnPosition = GetMousePosition();
	}

	player.update();
	update_balls();

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

	BeginMode2D(camera);

#define X(shader, name, address, type)\
SetShaderValue(shader, u_ ## name ## _loc, address, type);
	UNIFORMS
#undef X

	BeginShaderMode(ground_shader);

	for (int i = 0; i < NUM_BODIES; i++) {
		SetShaderValue(ground_shader, bodies[i].u_position_loc, &bodies[i].position, SHADER_UNIFORM_VEC2);
		SetShaderValue(ground_shader, bodies[i].u_radius_loc, &bodies[i].radius_m, SHADER_UNIFORM_FLOAT);
		SetShaderValue(ground_shader, bodies[i].u_mass_loc, &bodies[i].mass_kg, SHADER_UNIFORM_FLOAT);
	}

	DrawRectangle(0, 0, screen_dims.x, screen_dims.y, WHITE);
	EndShaderMode();

	draw_bodies();

	draw_balls();

	player.draw();

	DrawCircle(0, 0, 10, RED);
	DrawCircle(960, 540, 10, BLUE);
	DrawLine(DebugLine.x, DebugLine.y, DebugLine.z, DebugLine.w, RED);
	draw_preview();

	EndMode2D();

	DrawFPS(10, 10);

#if 0
	imgui();
#endif

	EndDrawing();
}

void load_shaders_and_font() {
	ground_shader = LoadShader(0, shader_dir "shader.fs");

	ground_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(ground_shader, "matModel");

#define X(shader, name, address, type)\
u_ ## name ## _loc = GetShaderLocation(shader, "u_" #name "\0" );
	UNIFORMS
#undef X
		//light locations
		for (int i = 0; i < NUM_BODIES; i++) {
			char position_name[32] = "u_bodies[x].position\0";
			char mass_name[32] = "u_bodies[x].mass\0";
			char radius_name[32] = "u_bodies[x].radius\0";

			sprintf(position_name, "u_bodies[%i].position", i);
			sprintf(mass_name, "u_bodies[%i].mass", i);
			sprintf(radius_name, "u_bodies[%i].radius", i);

			bodies[i].u_position_loc = GetShaderLocation(ground_shader, position_name);
			bodies[i].u_mass_loc = GetShaderLocation(ground_shader, mass_name);
			bodies[i].u_radius_loc = GetShaderLocation(ground_shader, radius_name);
		}
}

int main() {
	
	ImGuiContext* c = ImGui::CreateContext();
	ImGui::SetCurrentContext(c);
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	InitWindow(1, 1, "Censer");
	reset();
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	ImGui_ImplRaylib_Init();
	editor_mode = true;

	load_shaders_and_font();

	

	//SetCameraMode(player_camera, CAMERA_PERSPECTIVE);

	SetTargetFPS(144);

	static float lag_s = 0.0f;

	//HideCursor();
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

