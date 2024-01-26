//issue with selecting models that have just been created?
//add more enemies that are fun to fight
//perf: replace "immediate" drawing with instanced drawing(enemies, bullets)
//level autosave

//MONTH TASKS:
// what do I want the text system to be? 
//display 3d text in the world, one way conversation with characters where they can tell you stuff, interact with objects that can give you flavor text
//probably just have trigger zones that emit particles that display a string when you press an activation key and it intersects with the box
//potential problems: could get text stuck in the world or be hard to read due to bad angles or something

// difference between external text and internal text ie. someone talking to you vs
//someone communicating telepathically or reading something

//Grapple discussion:
//grapple wanted attributes:
//high skill ceiling
//rewards correct timing with speed

//pull speed is increased with time in grapple, this prevents spamming
//have a grapple length mechanic where you cannot increase length past when you started grappling
//cannot


//skybox and other visual elements/ shaders : 5 days
//create audio system : 2 days
//find and add sounds and music : 1 day
//create temp models and anims for player hands: 1 day
//fix game breaking bugs: 1 day
//create animation system that loads and blends keyframes of animations, able to play different anims when something happens : 1 week
//create temp models and anims for enemies : 2 days


//sound fx list
//spawning noise (grinding noise or some sort of slimy pop noise)
//sword swing noise (woosh)
//ambient enemy moaning that plays randomly
//grapple hit noise (clamp/ crunch)
//enemy melee wind up noise (yell or something)
//hit reaction noise for player (crunch or scratch noise)
//sword hit noise (meaty slice)
//grapple stretch noise
//gun shoot noise (sparkling magic noise)


//sparkling noise for diologue?
//footstep noise for enemies?

//notes on combat:
// add ambient sounds for enemies (know they are nearby
// adjust the buff perams and make sure they work!
//way to gain health
//add visual for damage taking

//THINGZ TO DO RN!!!
//balance level 
//make the level look better, add more billboards, textures etc. Things to talk to
//nuclear sign in bunker

//faustian bargin stuff

//LONG TERM
//music track(1)
//entities with visual todos
//load in seprate thread
//modelling and texturing enemies
//Attack anim for enemies
//ccd for bullets
//render tech that valve uses for far away objects
//fix depth precision
//some sort of AO
//shadows

//Remember config.h for raylib!!//

//Things to do today mar 12 2023
//sync with repo
//get random pitch shifting to work
//get panning audio to work
//add a band pass filter that scales frequency removel with distance
//audio debugging tools?

//fix immediate debug print to screen
//fix a bunch of broken rotation stuff

#include "pch.h"
#include "LevelFile.h"
#include "common.h"
#include "Entity.h"
#include "debug_draw.h"
#include "collision_manager.h"
#include "audio_manager.h"
#include "imgui-impl-raylib/imgui_impl_raylib.h"
#include "imgui-impl-raylib/imgui_impl_raylib_config.h"
#include "imgui-1.73/imgui.h"
#include "imgui-1.73/imconfig.h"
#include "particle.h"
#include "config.h"

level_t level;
int killed_enemies = 0;
float starting_widget_len = 10.0f;
float starting_widget_width = 5.0f;
WIDGET current_widget = WIDGET_TRANSLATE;
bool mult_select = false;
unordered_set<unsigned int> selected_entities;
unsigned int selected_light = 0;
bool imgui_hovered = false;
Shader ground_shader;
Model skybox;

const float cam_far = RL_CULL_DISTANCE_FAR;
const float cam_near = RL_CULL_DISTANCE_NEAR;
#define UNIFORMS \
X(ground_shader, clear_col, &level.clearColour, SHADER_UNIFORM_VEC3)\
X(ground_shader, fog_min, &level.fogLevelMin, SHADER_UNIFORM_FLOAT)\
X(ground_shader, fog_max, &level.fogLevelMax, SHADER_UNIFORM_FLOAT)\
X(ground_shader, cam_pos, &player_camera.position, SHADER_UNIFORM_VEC3)\
X(ground_shader, cam_far, &cam_far, SHADER_UNIFORM_FLOAT)\
X(ground_shader, cam_near, &cam_near, SHADER_UNIFORM_FLOAT)\
X(ground_shader, time, &game_time_s, SHADER_UNIFORM_FLOAT)\
X(ground_shader, log_depth, &game_time_s, SHADER_UNIFORM_FLOAT)\
X(ground_shader, drawing_far_plane, &drawing_far_plane, SHADER_UNIFORM_INT)\

#define X(shader, name, address, type) unsigned int u_ ## name ## _loc;
UNIFORMS
#undef X

#if ATGM_TESTING
ATGM_t atgm;
#endif

player_t player;

Camera3D player_camera;

#define ENEMIES_ON 1
enemy_t enemies[MAX_ENEMIES];
enemy_manager_t enemy_manager;

bullet_manager_t bullet_manager;

collision_manager_t collision_manager;

particle_system_t particle_system;

audio_manager_t audio_manager;

//Editor
bool editor_mode = false;
bool debug_collision_detection = false;
bool debug_enemy = false;

bool paused = false;
float game_timescale = 1.0f;
float game_time_s = 0.0f;

debug_drawer_t debug_drawer;

bool extrap = true;

unsigned int screen_width = int(1920*1.7);
unsigned int screen_height = int(1080*1.7);
bool fullscreen = false;

input_state_t inputs;

//@HACK
bool healed = false;

void reset() {
	player.init();
	player.pos_m = level.start_pos;
	player.cam_pitch_r = level.start_pitch;
	player.cam_yaw_r = level.start_yaw;
	player.velocity_mps = Vector3{ 0, 0, 0 };

#if ENEMIES_ON
	enemy_manager.kill_all_enemies();
	level.reset_spawners();
#endif
	for(int i = 0; i < level.level_geo.size(); i++) {
		level.level_geo[i]->model->materials[0].shader = ground_shader;
	}
	healed = false;
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
			player.cam_yaw_r -= inputs.mouse_pos.x*player.mouseSensitivityX;
			player.cam_pitch_r -= inputs.mouse_pos.y*player.mouseSensitivityY;
		}

		if(set_mouse_pos) {
			SetMousePosition(screen_width/2, screen_height/2);
		}
	}
}

void editor_update() {
	if(inputs.input_down & INPUT_FLAGS::ENTER) {
		editor_mode = !editor_mode;
		if(editor_mode) {
			ShowCursor();
		}
		HideCursor();
	}

	Ray pointer = GetMouseRay(ndc_to_screen_coords(inputs.mouse_pos, Vector2{ (float)screen_width, (float)screen_height }), player_camera);

	static bool using_widget = false;
	static DIRECTION widget_dir = DIR_NONE;

	if(selected_entities.size() > 0) {
		level_geo_t* e = level.level_geo[*selected_entities.begin()];

		//choose widget
		if(inputs.input_down & INPUT_FLAGS::E) {
			current_widget = WIDGET_TRANSLATE;
		}

		if(inputs.input_down & INPUT_FLAGS::Z) {
			current_widget = WIDGET_ROTATE;
		}

		if(inputs.input_down & INPUT_FLAGS::Q) {
			current_widget = WIDGET_SCALE;
		}

		static Vector3 widget_pos = Vector3One();
		Vector3 sum_points = Vector3Zero();
		for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
			sum_points += level.level_geo[*it]->pos_m;
			widget_pos = sum_points*(1.0f/(float)selected_entities.size());
		}

		BoundingBox x_b;
		BoundingBox y_b;
		BoundingBox z_b;

		float widget_scale = fmax(0.05f, Vector3Length(player.pos_m - widget_pos)*0.02f);

		float widget_width = starting_widget_width*widget_scale;
		float widget_len = starting_widget_len*widget_scale;
		//check ray	against widget collision boxes
		if(editor_mode && !using_widget) {
			switch(current_widget)
			{
			case WIDGET_TRANSLATE: {
				x_b = { widget_pos - Vector3{0, widget_width/2, widget_width/2} , widget_pos + Vector3{widget_len, widget_width/2, widget_width/2} };
				y_b = { widget_pos - Vector3{widget_width/2,0, widget_width/2} , widget_pos + Vector3{ widget_width/2, widget_len, widget_width/2} };
				z_b = { widget_pos - Vector3{widget_width/2, widget_width/2, 0} , widget_pos + Vector3{ widget_width/2, widget_width/2, widget_len} };
				break;
			}
			case WIDGET_ROTATE: {
				Vector3 x = {0.5f*widget_width, 2*widget_width, 2*widget_width};
				Vector3 y = {2*widget_width, 0.5f*widget_width, 2*widget_width};
				Vector3 z = {2*widget_width, 2*widget_width, 0.5f*widget_width};
				x_b = { widget_pos - x, widget_pos + x };
				y_b = { widget_pos - y, widget_pos + y };
				z_b = { widget_pos - z, widget_pos + z };
				break;
			}
			case WIDGET_SCALE:
				x_b = { widget_pos - Vector3{0, widget_width/2, widget_width/2} , widget_pos + Vector3{widget_len, widget_width/2, widget_width/2} };
				y_b = { widget_pos - Vector3{widget_width/2,0, widget_width/2} , widget_pos + Vector3{ widget_width/2, widget_len, widget_width/2} };
				z_b = { widget_pos - Vector3{widget_width/2, widget_width/2, 0} , widget_pos + Vector3{ widget_width/2, widget_width/2, widget_len} };
				break;
			default:
				break;
			}
			float t = FLT_MAX;
			float tmin = FLT_MAX;
			Vector3 q;
			widget_dir = intersect_ray_AABB(pointer, x_b, &t, &q) ? DIR_X : DIR_NONE;
			if(widget_dir == DIR_X) {
				tmin = fmin(t, tmin);
			}
			widget_dir = intersect_ray_AABB(pointer, y_b, &t, &q) && t < tmin ? DIR_Y : widget_dir;
			if(widget_dir == DIR_Y) {
				tmin = fmin(t, tmin);
			}
			widget_dir = intersect_ray_AABB(pointer, z_b, &t, &q) && t < tmin ? DIR_Z : widget_dir;
#if 0
			debug_drawer.add_AABB(x_b, RED);
			debug_drawer.add_AABB(y_b, GREEN);
			debug_drawer.add_AABB(z_b, BLUE);
#endif
		}

		//select widget axis
		if(inputs.input_down & INPUT_FLAGS::LMB && editor_mode) {
			if(widget_dir != DIR_NONE) {
				using_widget = true;
			}
		}

		//use widget
		if(inputs.input_state & INPUT_FLAGS::LMB && editor_mode) {
			if(using_widget) {
				switch(current_widget)
				{
				case WIDGET_TRANSLATE: {
					Vector3 closest_points[2];
					closest_points_on_two_lines(pointer, Ray{ widget_pos, UNIT_VECS[widget_dir] }, closest_points);

					debug_drawer.add_ray(Ray{ widget_pos, UNIT_VECS[widget_dir] });
					//@bug debug_drawer.add_cross(closest_points[1], 1.0f);
					debug_drawer.add_cross(__COUNTER__, closest_points[1], 1.0f);

					static Vector3 starting_offset;
					Vector3 delta = Vector3Zero();
					if(inputs.input_down & INPUT_FLAGS::LMB) {
						starting_offset = widget_pos - closest_points[1];
					}

					switch(widget_dir)
					{
					case DIR_X:
						delta.x = closest_points[1].x - widget_pos.x + starting_offset.x;
						break;
					case DIR_Y:
						delta.y = closest_points[1].y - widget_pos.y + starting_offset.y;
						break;
					case DIR_Z:
						delta.z = closest_points[1].z - widget_pos.z + starting_offset.z;
						break;
					case DIR_NONE:
						break;
					}
					for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
						level.level_geo[*it]->pos_m += delta;
					}
					break;
				}
				case WIDGET_ROTATE: {
					Vector3 closest_point;
					closest_point = ray_plane_intersection(pointer, widget_pos, UNIT_VECS[widget_dir]);
					Vector3 dir = Vector3Normalize(closest_point - widget_pos);
					Vector3 vec = dir*widget_len;
					closest_point = widget_pos + vec;

					float angle;
					switch(widget_dir)
					{
					case DIR_X:
						angle = atan2f(dir.z, dir.y);
						break;
					case DIR_Y:
						angle = -atan2f(dir.z, dir.x);
						break;
					case DIR_Z:
						angle = atan2f(dir.y, dir.x);
						break;
					default:
						break;
					}

					if(selected_entities.size() == 1) {
						static float starting_angle;
						static float starting_entity_angle;
						static Vector3 starting_entity_axis;
						if(inputs.input_down & INPUT_FLAGS::LMB) {
							starting_angle = angle;
							starting_entity_angle = e->angle;
							starting_entity_axis = e->rot_axis;
						}
						//@TODO: BUG: there is a bug where if the angle is exactly
						//1.7 something rad then collision detection breaks

						Quaternion q = QuaternionFromAxisAngle(starting_entity_axis, starting_entity_angle);
						Quaternion q1 = QuaternionFromAxisAngle(UNIT_VECS[widget_dir], angle - starting_angle);

						Quaternion q3 = QuaternionMultiply(q, q1);
						Vector3 axis;
						float new_angle;
						QuaternionToAxisAngle(q3, &axis, &new_angle);

						for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
							level.level_geo[*it]->rot_axis = axis;
							level.level_geo[*it]->angle = new_angle; //round_up_to(starting_entity_angle + (angle - starting_angle), PI/8);
						}
					}
					else if(selected_entities.size() > 1) {
						static float starting_angle;
						if(inputs.input_down & INPUT_FLAGS::LMB) {
							starting_angle = angle;
						}

						Matrix m = MatrixRotate(UNIT_VECS[widget_dir], angle - starting_angle);
						for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {

							Vector3 v = level.level_geo[*it]->pos_m - widget_pos;
							level.level_geo[*it]->pos_m = Vector3Transform(v, m);
							level.level_geo[*it]->pos_m += widget_pos;
						}
					}
#if 0
					debug_drawer.add_cross(closest_point, 5);
#endif
					break;
				}
				case WIDGET_SCALE: {
					Vector3 closest_points[2];
					closest_points_on_two_lines(pointer, Ray{ widget_pos, UNIT_VECS[widget_dir] }, closest_points);

					debug_drawer.add_ray(Ray{ widget_pos, UNIT_VECS[widget_dir] });
#if 0
					debug_drawer.add_cross(closest_points[1], 1.0f);
#endif

					static Vector3 starting_offset;
					Vector3 delta = Vector3Zero();
					if(inputs.input_down & INPUT_FLAGS::LMB) {
						starting_offset = e->scale - closest_points[1];
					}

					switch(widget_dir)
					{
					case DIR_X:
						delta.x = closest_points[1].x - e->scale.x + starting_offset.x;
						break;
					case DIR_Y:
						delta.y = closest_points[1].y - e->scale.y + starting_offset.y;
						break;
					case DIR_Z:
						delta.z = closest_points[1].z - e->scale.z + starting_offset.z;
						break;
					case DIR_NONE:
						break;
					}
					for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
						level.level_geo[*it]->scale += delta*0.1f;
						level.level_geo[*it]->scale = Vector3Max(Vector3One()*EPSILON, level.level_geo[*it]->scale);
					}
					break;
				}
				default:
					break;
				}
			}
		}

		//stop using widget
		if(inputs.input_up & INPUT_FLAGS::LMB && editor_mode) {
			if(using_widget) {
				using_widget = false;
				for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
					level.level_geo[*it]->compute_bounding_box();
				}
			}
		}

		if(editor_mode) {

			Color axis_col[3] = { RED, GREEN, BLUE };
			for (int i = DIR_X; i <= DIR_Z; i++) { axis_col[i].a = 100; }

			if (widget_dir != DIRECTION::DIR_NONE) { axis_col[widget_dir].a = 255; }

			//draw widget
			switch(current_widget)
			{
			case WIDGET_TRANSLATE:
				for (int i = DIR_X; i <= DIR_Z; i++) {
					debug_drawer.add_vector(widget_pos, UNIT_VECS[i] * widget_len, 5.0f * widget_scale, 0.3f, axis_col[i]);
				}
				break;
			case WIDGET_ROTATE:
				for (int i = DIR_X; i <= DIR_Z; i++) {
					debug_drawer.add_circle(widget_pos, widget_len, UNIT_VECS[i], axis_col[i]);
				}
				break;
			case WIDGET_SCALE:
				float cyl_rad = 2.0f*widget_scale;
				float sph_rad = 3.0f*widget_scale;
				for (int i = DIR_X; i <= DIR_Z; i++) {
					debug_drawer.add_cylinder(widget_pos, UNIT_VECS[i], cyl_rad, cyl_rad, widget_len, axis_col[i]);
					debug_drawer.add_sphere(widget_pos + UNIT_VECS[i] * widget_len, sph_rad, axis_col[i]);
				}
				break;
			}
		}
	}

	//Select closest level geo
	if(inputs.input_down & INPUT_FLAGS::LMB && editor_mode && !using_widget && !imgui_hovered) {
		float min = FLT_MAX;
		int min_i = 0;
		bool hit = false;
		for(int i = 0; i < level.level_geo.size(); i++) {
			OBB_t b = level.level_geo[i]->OBB;

			float tmin = FLT_MAX;
			Vector3 intersection_pt;
			hit = intersect_ray_OBB(pointer, level.level_geo[i]->OBB, &tmin, &intersection_pt) || hit;
			if(tmin < min) {
				min = tmin;
				min_i = i;
			}
		}
		if(hit) {
			if(!ImGui::GetIO().KeyCtrl) {
				selected_entities.clear();
			}
			selected_entities.insert(min_i);
		}
	}

	//Draw selected objets
	if(editor_mode) {
		for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
			OBB_t b = level.level_geo[*it]->OBB;
			debug_drawer.add_OBB(b);
		}

#if 0
		debug_drawer.add_cross(level.lights[selected_light].pos, 10);
#endif
	}

	//silly stuff here
#if 0
	OBB_t o1;
	OBB_t o2;
	if(selected_entities.size() == 2) {
		bool other = true;
		for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
			if(other) {
				o1 = level.entities[*it]->OBB;
			}
			else 
			{
				o2 = level.entities[*it]->OBB;
			}
			other = false;
		}
		intersection_info_t info;

		//info = intersection_OBB_OBB(o1, o2);

		Matrix m_a = MatrixLookAt(Vector3Zero(), -o1.u[2], o1.u[1]);
		Matrix m_b = MatrixLookAt(Vector3Zero(), -o2.u[2], o2.u[1]);

		Quaternion q_a = QuaternionFromMatrix(m_a);
		Quaternion q_b = QuaternionFromMatrix(m_b);

		Vector3 x_axis = Vector3RotateByQuaternion(UNIT_VECS[DIR_X], q_a);
		Vector3 y_axis = Vector3RotateByQuaternion(UNIT_VECS[DIR_Y], q_a);
		Vector3 z_axis = Vector3RotateByQuaternion(UNIT_VECS[DIR_Z], q_a);
		debug_drawer.add_line(o1.c, o1.c + x_axis*1.0f, RED, 0.0f, false);
		debug_drawer.add_line(o1.c, o1.c + y_axis*1.0f, GREEN, 0.0f, false);
		debug_drawer.add_line(o1.c, o1.c + z_axis*1.0f, BLUE, 0.0f, false);

	//	info = intersection_OBB_OBB(o1, o2);
		info.colliding = collision_OBB_OBB(o1, o2);
		info.intersec_vector_m = UNIT_VECS[DIR_X];

		//debug_drawer.add_ray(Ray{o1.c - info.intersec_vector_m*500 , info.intersec_vector_m }, GREEN, 0.0f, false);
		debug_drawer.add_vector(o1.c, info.intersec_vector_m, 0.2f, 0.3f, info.colliding ? RED : MAGENTA, 0.0f, false);

	//	debug_drawer.add_vector(Vector3{ 1,0,0 }, Vector3Normalize(info.intersec_vector_m), 0.2f, 0.3f, BLUE, 0.0f, false);

		if(inputs.input_down & INPUT_FLAGS::Q) {
			level.entities[*selected_entities.begin()]->pos_m += info.intersec_vector_m;
			level.entities[*selected_entities.begin()]->compute_bounding_box();
		}
	}
#endif
	//draw origin
	if(editor_mode) {
		debug_drawer.add_axes(Vector3Zero(), 1.0f);
		debug_drawer.add_axes(level.start_pos, 1.0f);
	}

	//editor teleport
	if(editor_mode) {
		if(inputs.input_down & INPUT_FLAGS::T) {
			//cast ray and teleport us there
			float tmin;
			Vector3 intersec_pt;
			bool hit = collision_manager.cast_ray(pointer, OBJECT_TYPE::TYPE_LEVEL_GEO, &tmin, &intersec_pt);

			if(hit) {
				player.pos_m += (intersec_pt - player.pos_m)*(0.8);
			}
		}
	}

	if(inputs.input_down & INPUT_FLAGS::RIGHT) {
		game_timescale *= 2.0f;
		debug_drawer.add_string(Vector3{ 0.2f, 0.2f, 0 }, 70, false, RED, 5.0f, "Game speed: %.2f", game_timescale);
	}

	if(inputs.input_down & INPUT_FLAGS::LEFT) {
		game_timescale *= 0.5f;
		debug_drawer.add_string(Vector3{ 0.2f, 0.2f, 0 }, 70, false, RED, 5.0f, "Game speed: %.2f", game_timescale);
	}
}

//When this is called we know that the 2 primitives are overlapping
//collision responce/resolution
void my_on_overlap(collision_info_t info) {
	collidable_t* a = &info.a;
	collidable_t* b = &info.b;

	if(debug_collision_detection) {
		debug_drawer.add_AABB(a->b, RED);
		debug_drawer.add_AABB(b->b, RED);
	}
	void* maybe_e;
	void* maybe_p;

	//player vs level geo
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_PLAYER, OBJECT_TYPE::TYPE_LEVEL_GEO, &maybe_p, &maybe_e)) {
		player_t* p = (player_t*)maybe_p;
		level_geo_t* e = (level_geo_t*)maybe_e;
		if(editor_mode && p->editor_noclip) {
			return;
		}

		if(a->col_type == COL_BOTTOM_BB) {
			intersection_info_t info;
			if(e->collision) {
				info = intersection_AABB_OBB(p->bottom_bounding_box, e->OBB);

				if(acosf(Vector3DotProduct(Vector3Normalize(info.intersec_vector_m), UNIT_VECS[DIR_Y]))*RAD2DEG < p->ground_angle_deg) {
					p->state = GROUND;
				}
			}
			//debug_drawer.add_vector(p->pos_m + p->look_dir*3 + UNIT_VECS[DIR_Y]*3, info.intersec_vector_m, 0.5f, 0.1f);

			//debug_drawer.add_string(Vector3{ 0.5f, 0.5f, 0 }, 60, false, GREEN, 0.0f, "ground angle = %f", acosf(Vector3DotProduct(Vector3Normalize(info.intersec_vector_m), UNIT_VECS[DIR_Y]))*RAD2DEG);
			return;
		} else if(a->col_type == COL_PLAYER) {
			intersection_info_t collide;
			if(e->collision) {
				collide = intersection_AABB_OBB(p->bounding_box, e->OBB);
			}
			//debug_drawer.add_vector(p->pos_m + p->look_dir*3 + UNIT_VECS[DIR_Y]*3, collide.intersec_vector_m, 0.5f, 0.1f, RED, 0.0f, false);

			p->pos_m += collide.intersec_vector_m;
			Vector3 intersection_normal = Vector3Normalize(collide.intersec_vector_m);

			if(Vector3Length(intersection_normal) > EPSILON) {
				p->velocity_mps = Vector3CrossProduct(intersection_normal, Vector3CrossProduct(p->velocity_mps, intersection_normal));
			}
			if(acosf(Vector3DotProduct(intersection_normal, UNIT_VECS[DIR_Y]))*RAD2DEG < p->ground_angle_deg) {
				//p->state = GROUND;
			}
			else {
				p->boost_timer.accum_s = -1.0f;
			}
			return;
		}
	}

	void* maybe_s;
	//player vs. spawn trigger
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_PLAYER, OBJECT_TYPE::TYPE_SPAWNER, &maybe_p, &maybe_s)) {
		enemy_spawner_t* s = (enemy_spawner_t*)maybe_s;
		s->spawn();
		return;
	}

	//Player vs. text trigger
	void* maybe_t;
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_PLAYER, OBJECT_TYPE::TYPE_TEXT_TRIGGER, &maybe_p, &maybe_t)) {
		text_trigger_t* t = (text_trigger_t*)maybe_t;
		player_t* p = (player_t*)maybe_p;
		t->disappear_timer.reset();
		if(p->look_trigger_active) {
			t->displaying_text = true;
		}
		return;
	}

	//bullet vs. level geo
	void* maybe_b;
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_BULLET, OBJECT_TYPE::TYPE_LEVEL_GEO, &maybe_b, &maybe_e)) {
		bullet_t* b = (bullet_t*)maybe_b;
		level_geo_t* e = (level_geo_t*)maybe_e;
		particle_system.activate_particles(3, b->pos_m, -Vector3Normalize(b->velocity_mps)*1.0f, GREEN, 1.0f, true, 0.1f);

		b->deactivate();
		return;
	}

	//bullet vs. enemy
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_BULLET, OBJECT_TYPE::TYPE_ENEMY, &maybe_b, &maybe_e)) {
		bullet_t* b = (bullet_t*)maybe_b;
		enemy_t* e = (enemy_t*)maybe_e;
		e->take_damage(b->damage);
		b->deactivate();
		particle_system.activate_particles(b->damage, e->pos_m, -Vector3Normalize(b->velocity_mps)*10, RED, 1.0f, true);
		audio_manager.add_sound_fx_positional(SOUNDS::evasion2, 0.05f, e->pos_m, true);
		return;
	}

	//player vs. enemy bullet
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_BULLET, OBJECT_TYPE::TYPE_PLAYER, &maybe_b, &maybe_p)) {
		bullet_t* b = (bullet_t*)maybe_b;
		player_t* p = (player_t*)maybe_p;

		p->take_damage(b->damage);
		b->deactivate();
		return;
	}

	//bullet vs. enemy bullet
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_BULLET, OBJECT_TYPE::TYPE_BULLET, &maybe_b, &maybe_p)) {
		bullet_t* b = (bullet_t*)maybe_b;
		bullet_t* p = (bullet_t*)maybe_p;

		if(b->friendly) {
			p->deactivate();
		}
		else {
			b->deactivate();
		}
		return;
	}

	void* maybe_e1;
	void* maybe_e2;
	
	//enemy vs. enemy
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_ENEMY, OBJECT_TYPE::TYPE_ENEMY, &maybe_e1, &maybe_e2)) {
		enemy_t* e1 = (enemy_t*)maybe_e1;
		enemy_t* e2 = (enemy_t*)maybe_e2;
		intersection_info_t collide;
		collide = intersection_AABB(e1->bounding_box, e2->bounding_box);
		if(collide.colliding) {
			e1->pos_m -= collide.intersec_vector_m;
			Vector3 intersection_normal = Vector3Normalize(collide.intersec_vector_m);
			e1->velocity_mps = Vector3CrossProduct(intersection_normal, Vector3CrossProduct(e1->velocity_mps, intersection_normal));
		}
		return;
	}

	//enemy vs. level geo
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_ENEMY, OBJECT_TYPE::TYPE_LEVEL_GEO, &maybe_e, &maybe_b)) {
		enemy_t* e = (enemy_t*)maybe_e;
		level_geo_t* b = (level_geo_t*)maybe_b;

		intersection_info_t collide;
		OBB_t obb;
		convert_AABB_to_OBB(e->bounding_box, &obb);
		collide = intersection_OBB_OBB(obb, b->OBB);
		if(collide.colliding) {
			e->pos_m += collide.intersec_vector_m;
			Vector3 intersection_normal = Vector3Normalize(collide.intersec_vector_m);
			e->velocity_mps = Vector3CrossProduct(intersection_normal, Vector3CrossProduct(e->velocity_mps, intersection_normal));
			e->on_ground = true;
		}
		return;
	}

	//enemy vs. buff volume
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_ENEMY, OBJECT_TYPE::TYPE_ENEMY_BUFF_VOL, &maybe_e, &maybe_b)) {
		enemy_t* e = (enemy_t*)maybe_e;

		e->apply_buff();
		return;
	}

	//player vs. enemy
	if(type_check_collidables(&a, &b, OBJECT_TYPE::TYPE_PLAYER, OBJECT_TYPE::TYPE_ENEMY, &maybe_p, &maybe_e)) {
		player_t* p = (player_t*)maybe_p;
		enemy_t* e = (enemy_t*)maybe_e;
		if(a->col_type == COL_PLAYER_MELEE) {
			particle_system.activate_particles(50, e->pos_m, player.strafe_dir*10.0f, RED, 1.0f, true);
			audio_manager.add_sound_fx_positional(SOUNDS::hitsquish, 0.1f, e->pos_m, true);
			if(e->type != ENEMY_TYPE::ENEMY_FLYER) {
				int ammo_gained = (int)(player.melee_damage*player.ammo_gained_per_melee_damage);
				player.ammo_count += ammo_gained;

				float time_to_absorb = 0.3f;
				Vector3 vec = player_camera.position - e->pos_m;
				particle_system.activate_particles(ammo_gained, e->pos_m, Vector3Normalize(vec)*(Vector3Length(vec)/time_to_absorb), GREEN, time_to_absorb, false, 0.3f, 0.0f);
			}
			e->take_damage(player.melee_damage);
		}
		else {
			p->take_damage(e->melee_damage);
		}
		return;
	}

//"Unhandled collision, you may have added a new collision type and not added any code here."
	assert(false);
}

void update() {
	static timer_t t;
	t.freq_hz = 1.0f/1.0f;
	bool activate = t.tick(dt_s);
	if(activate) {
		//particle_system.activate_particles(10, level.startPos, UNIT_VECS[DIR_Y], RED, 5.0f);
		t.reset();
	}

	player.update();

	for(int i = 0; i < MAX_BULLETS; i++) {
		bullet_manager.bullets[i].update();
	}

#if ENEMIES_ON
	if(!editor_mode) {
		Vector3 enemy_pos = Vector3Zero();
		if(debug_enemy) {
#if 0
			debug_drawer.add_cross(enemy_pos, 1.0f, RED);
#endif
			Ray r = { player_camera.position, player.look_dir };
			static Vector3 q = Vector3Zero();
			float t;
			collision_manager.cast_ray(r, OBJECT_TYPE::TYPE_LEVEL_GEO, &t, &q);
			enemy_pos = q;
		}
		else {
			enemy_pos = player.pos_m + UNIT_VECS[DIR_Y]*(player.height_m/2);
		}
		enemy_manager.update_enemies(enemy_pos);
	}
#endif

	particle_system.update_particles();
	level.update_text_triggers();
	audio_manager.update(player_camera.position, player.look_dir);

	if(inputs.input_state & INPUT_FLAGS::EXIT) {
		CloseWindow();
	}

	if(inputs.input_down & INPUT_FLAGS::R) {
		reset();
	}
	
	//add colliders to collsion manager
	collision_manager.clear_collidables();

	//@TODO: should maybe move this into their respective structs
	//player.add_collidables(); etc.

	//@HACK!! remove this
	if(player.pos_m < Vector3{ -31, 121, -936 } && player.pos_m > Vector3{ -55, 98, -952 } && !healed) {
		player.health = player.max_health;
		audio_manager.add_sound_fx(SOUNDS::Magic1, 1.0f, true);
		healed = true;
	}

	{
		//Player
		uint16_t player_collides_with = COL_ENEMY | COL_LEVEL_GEO | COL_ENEMY_BULLET | COL_ENEMY_MELEE | COL_SPAWN_TRIGGER;
		primitive_t player_prim;
		player_prim.prim_type = PRIM_TYPE::PRIM_AABB;
		player_prim.prim.aabb = player.bounding_box;
		collision_manager.add_collidable(&player, player.bounding_box, OBJECT_TYPE::TYPE_PLAYER, player_prim, COL_PLAYER, player_collides_with);
	}

	{
		//player look activator
		uint16_t look_collides_with = COL_TEXT_TRIGGER;
		primitive_t look_prim;
		look_prim.prim_type = PRIM_TYPE::PRIM_OBB;
		OBB_t obb;

		obb.u[0] = player.look_dir;
		obb.u[1] = Vector3Normalize(get_perp_vec(obb.u[0]));
		obb.u[2] = Vector3Normalize(Vector3CrossProduct(obb.u[0], obb.u[1]));

		obb.e[0] = 2.0f;
		obb.e[1] = 0.1f;
		obb.e[2] = 0.1f;

		obb.c = player_camera.position + player.look_dir*obb.e[0]*0.5f;
		//debug_drawer.add_OBB(obb, RED, 0.0f, false);
		look_prim.prim.obb = obb;
		BoundingBox bb;
		convert_OBB_to_AABB(obb, &bb);
		collision_manager.add_collidable(&player, bb, OBJECT_TYPE::TYPE_PLAYER, look_prim, COL_PLAYER_TRIGGER, look_collides_with);
	}

	{
		//level geo
		uint16_t geo_collides_with = COL_FRIEND_BULLET | COL_PLAYER | COL_ENEMY;
		primitive_t geo_prim;
		geo_prim.prim_type = PRIM_TYPE::PRIM_OBB;
		for(auto i : level.level_geo) {
			if(i->collision) {
				//debug_drawer.add_string(i->pos_m, 100.0f, true, RED, 0.0f, "%p",i );
				geo_prim.prim.obb = i->OBB;
				collision_manager.add_collidable(i, i->bb, OBJECT_TYPE::TYPE_LEVEL_GEO, geo_prim, COL_LEVEL_GEO, geo_collides_with);
			}
		}
	}

	if(!editor_mode) {
		{
			//enemy spawners
			//@todo: make this uint16 a typedef and do a compile check to make sure all our object bit flags fit inside it
			uint16_t spawner_collides_with = COL_PLAYER;
			primitive_t spawner_prim;
			spawner_prim.prim_type = PRIM_TYPE::PRIM_AABB;
			for(int i = 0; i < MAX_SPAWNERS; i++) {
				enemy_spawner_t* s = &level.spawners[i];
				if(s->active) {
					spawner_prim.prim.aabb = s->bb;
					collision_manager.add_collidable(s, s->bb, OBJECT_TYPE::TYPE_SPAWNER, spawner_prim, COL_SPAWN_TRIGGER, spawner_collides_with);
				}
			}
		}
	}

	{
		//bullets and bullet shields 
		uint16_t enemy_bullet_collides_with = COL_LEVEL_GEO | COL_PLAYER;
		uint16_t friend_bullet_collides_with = COL_ENEMY;
		uint16_t friend_bullet_shield_collides_with = COL_ENEMY_BULLET;
		primitive_t bul_prim;
		bul_prim.prim_type = PRIM_TYPE::PRIM_SPHERE;
		for(int i = 0; i < MAX_BULLETS; i++) {
			bullet_t* bul = &bullet_manager.bullets[i];
			if(bul->active) {
				BoundingBox b = { bul->pos_m - Vector3One()*bul->radius_m, bul->pos_m + Vector3One()*bul->radius_m };
				bul_prim.prim.sph = { bul->pos_m, bul->radius_m };
				if(bul->friendly) {
					collision_manager.add_collidable(bul, b, OBJECT_TYPE::TYPE_BULLET, bul_prim, COL_FRIEND_BULLET, friend_bullet_collides_with);
					b = { bul->pos_m - Vector3One()*bul->shield_radius_m, bul->pos_m + Vector3One()*bul->shield_radius_m };
					bul_prim.prim.sph.r = bul->shield_radius_m;
					collision_manager.add_collidable(bul, b, OBJECT_TYPE::TYPE_BULLET, bul_prim, COL_FRIEND_SHIELD, friend_bullet_shield_collides_with);
				}
				else {
					collision_manager.add_collidable(bul, b, OBJECT_TYPE::TYPE_BULLET, bul_prim, COL_ENEMY_BULLET, enemy_bullet_collides_with);
				}
			}
		}
	}
	
#if ENEMIES_ON
	if(!editor_mode) {
		enemy_manager.add_collidables();
	}
#endif

	level.add_text_collidables();

	{
		//player bottom bounding box
		uint16_t player_bottom_BB_collides_with = COL_LEVEL_GEO;
		primitive_t bbb_prim;
		bbb_prim.prim_type = PRIM_TYPE::PRIM_AABB;
		bbb_prim.prim.aabb = player.bottom_bounding_box;
		collision_manager.add_collidable(&player, player.bottom_bounding_box, OBJECT_TYPE::TYPE_PLAYER, bbb_prim, COL_BOTTOM_BB, player_bottom_BB_collides_with);
	}

	{
		//player melee hurtbox
		if(player.melee_active_timer.accum_s == 1/player.melee_active_timer.freq_hz) {
			uint16_t player_melee_collides_with = COL_ENEMY;
			primitive_t p_melee_prim;
			p_melee_prim.prim_type = PRIM_TYPE::PRIM_OBB;
			p_melee_prim.prim.obb = player.melee_hurtbox;
			BoundingBox bb;
			convert_OBB_to_AABB(player.melee_hurtbox, &bb);
			collision_manager.add_collidable(&player, bb, OBJECT_TYPE::TYPE_PLAYER, p_melee_prim, COL_PLAYER_MELEE, player_melee_collides_with);
			//debug_drawer.add_OBB(player.melee_hurtbox, PURPLE, 1.0f, true);
			//debug_drawer.add_OBB(player.melee_hurtbox, RED, 0.0f, true);

			//debug_drawer.add_vector(player.melee_hurtbox.c, player.melee_hurtbox.u[0], 0.2f, 0.1f, RED, 1.0f, false);
			//debug_drawer.add_vector(player.melee_hurtbox.c, player.melee_hurtbox.u[1], 0.2f, 0.1f, GREEN, 1.0f, false);
			//debug_drawer.add_vector(player.melee_hurtbox.c, player.melee_hurtbox.u[2], 0.2f, 0.1f, BLUE, 1.0f, false);
		}
	}

	//TESTING AUDIO PANNING
#if 0

	Vector3 sound_pos = { sin(game_time_s)*10, 0.0f, cos(game_time_s)*10 };
	//Vector3 sound_pos = { 0,0 };

	static float timer = 0.0f;
	timer += dt_s;

	if (timer > 0.7) {
		audio_manager.add_sound_fx_positional(SOUNDS::laser, 1.0f, sound_pos, false);
		timer = 0.0f;
	}

	debug_drawer.add_sphere(sound_pos, 0.2f);
#endif
	


	//check all collisions
	collision_manager.check_world_collisions();
}

void imgui() {
	// IMGUI
	//@WARNING:This could cause some entities to not be shown
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
			if(ImGui::BeginTabItem("Entity"))
			{
				ImGui::Text("Entity List");
				for(unsigned int i = 0; i < level.level_geo.size(); i++) {
					listbox_display[i] = new char[to_string(i).length() + 1];
					strcpy(listbox_display[i], to_string(i).c_str());
				}

				if(ImGui::ListBoxHeader("", int(level.level_geo.size()), 10)) {
					for(unsigned int i = 0; i < level.level_geo.size(); i++) {
						if(ImGui::Selectable(listbox_display[i], selected_entities.find(i) != selected_entities.end())) {
							if(!ImGui::GetIO().KeyCtrl) {    // Clear selection when CTRL is not held
								selected_entities.clear();
							}
							selected_entities.insert(i);
						}
					}
					ImGui::ListBoxFooter();
				}

				static Vector3 deltaPos = { 0,0,0 };
				static Vector3 deltaScale = { 0,0,0 };
				static Vector3 deltaAxis = { 0,0,0 };
				static float deltaAngle = 0.0f;
				bool edited = false;
				if(selected_entities.size() == 0) {}
				else if(selected_entities.size() == 1) {
					level_geo_t& selected = *level.level_geo[*(selected_entities.begin())];

					edited |= ImGui::DragFloat3("Scale", (float*)(&selected.scale), 0.1f, EPSILON, 5000.0f);
					edited |= ImGui::DragFloat3("Position", (float*)(&selected.pos_m), 0.1f, -10000.0f, 10000.0f);
					edited |= ImGui::DragFloat3("RotAxis", (float*)(&selected.rot_axis), 0.05f, -1.0f, 1.0f);
					edited |= ImGui::DragFloat("Angle", &selected.angle, 0.05f, 0, PI);
					ImGui::Text("deg = %f", selected.angle*(float)RAD2DEG);
					edited |= ImGui::Checkbox("Collidable", &selected.collision);
					ImGui::Text(selected.model_file_name);
					ImGui::Text(selected.tex_file_name);
					Vector3* box_dims = (Vector3*)selected.OBB.e;
					ImGui::Text("bounding box dims:\n x = %.2f, y = %.2f, z = %.2f", box_dims->x, box_dims->y, box_dims->z);
				}
				else {
					edited |= ImGui::DragFloat3("Scale", (float*)(&deltaScale), 0.1f, -5000.0f, 5000.0f);
					edited |= ImGui::DragFloat3("Position", (float*)(&deltaPos), 0.1f, -5000.0f, 5000.0f);
					edited |= ImGui::DragFloat3("RotAxis", (float*)(&deltaAxis), 0.05f, -1.0f, 1.0f);
					edited |= ImGui::DragFloat("Angle", &deltaAngle, 0.05f, 0, PI);
				}

				if(edited) {
					for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
						level.level_geo[*it]->pos_m += deltaPos;
						level.level_geo[*it]->scale += deltaScale;
						level.level_geo[*it]->scale = Vector3Max(Vector3One()*EPSILON, level.level_geo[*it]->scale);
						level.level_geo[*it]->rot_axis += deltaAxis;
						level.level_geo[*it]->angle += deltaAngle;
						level.level_geo[*it]->rot_axis = Vector3Normalize(level.level_geo[*it]->rot_axis);
						level.level_geo[*it]->compute_bounding_box();
					}
				}
				else {
					deltaPos = { 0,0,0 };
					deltaScale = { 0,0,0 };
					deltaAxis = { 0,0,0 };
					deltaAngle = 0.0f;
				}
				if(selected_entities.size() != 0) {
					if(ImGui::Button("Delete") && level.level_geo.size() > 1 && selected_entities.size() < level.level_geo.size()) {
						//@LEAK: If an entity is deleted that points to model data, model it will be leaked
						for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
							delete level.level_geo[*it];
							level.level_geo[*it] = nullptr;
						}
						for(unsigned int i = 0; i < level.level_geo.size(); i++) {
							if(level.level_geo[i] == nullptr) {
								level.level_geo.erase(level.level_geo.begin() + i);
								i--;
							}
						}
						selected_entities.clear();
					}
					ImGui::SameLine();
					if(ImGui::Button("Clone")) {
						for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
							level_geo_t* e = new level_geo_t;
							*e = *level.level_geo[*it];
							level.level_geo.push_back(e);
						}
					}
					if(ImGui::Button("Flip x")) {

						static Vector3 flip_pos = Vector3One();
						Vector3 sum_points = Vector3Zero();
						for(auto it = selected_entities.begin(); it != selected_entities.end(); it++) {
							sum_points += level.level_geo[*it]->pos_m;
							flip_pos = sum_points*(1.0f/(float)selected_entities.size());
						}

						for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
							Vector3 temp = level.level_geo[*it]->pos_m - flip_pos;
							temp.x *= -1.0f;
							level.level_geo[*it]->pos_m = temp + flip_pos;
							level.level_geo[*it]->rot_axis = level.level_geo[*it]->rot_axis*-1.0f;
							level.level_geo[*it]->compute_bounding_box();
						}
					}
				}
				static unsigned int num_models = 0;
				static unsigned int selected_model_file = 0;
				static char* model_files[MAX_FILES_IN_DIR] = {};
				num_models = list_of_files_in_dir(model_files, asset_dir, ".obj");
				ImGui::Text("\nMeshes");
				ImGui::ListBox("rats", (int*)(&selected_model_file), model_files, num_models, 10);
				if(ImGui::Button("Add")) {
					level_geo_t* e = new level_geo_t;
					level.load_model_for_level_geo(model_files[selected_model_file], e);
					e->pos_m = player.pos_m + player.look_dir*3.0f;
					e->compute_bounding_box();
				}

				if(ImGui::Button("Replace with selected model")) {
					for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
						level_geo_t* g = level.level_geo[*it];
						level.load_model(model_files[selected_model_file], g);
						g->compute_bounding_box();
					}
				}

				ImGui::EndTabItem();
			}
			if(selected_entities.size() > 0 && ImGui::BeginTabItem("geo texturing")) {
				bool edited = false;
				static bool first = true;
				static unsigned int num_tex = 0;
				static unsigned int selected_tex = -1;
				static char* tex_files[MAX_FILES_IN_DIR] = {};

				if(ImGui::Button("refresh tex list") || first) {
					num_tex = list_of_files_in_dir(tex_files, asset_dir, ".png");
					first = false;
				}
				ImGui::ListBox("", (int*)(&selected_tex), tex_files, num_tex, 10);

				if(selected_tex != -1 && ImGui::Button("change tex")) {
#if 1
					for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
						level.load_tex(tex_files[selected_tex], level.level_geo[*it]);
					}
#else
					char tex_path_name[CUTE_FILES_MAX_PATH];
					path_concat(asset_dir, tex_files[selected_tex], tex_path_name, CUTE_FILES_MAX_PATH);

					Texture2D tex = LoadTexture(tex_path_name);
					for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
						Model* m = level.level_geo[*it]->model;
						SetMaterialTexture(&m->materials[0], MAP_DIFFUSE, tex);
					}
#endif
				}

				if(ImGui::Button("bad uv project")) {
					edited |= true;
				}
				Vector2* offset = &level.level_geo[*selected_entities.begin()]->tex_offset;
				Vector2* scale = &level.level_geo[*selected_entities.begin()]->tex_scale;

				Vector2 prev_offset = *offset;
				Vector2 prev_scale = *scale;
				edited |= ImGui::SliderFloat2("offset", (float*)offset, -1.0f, 1.0f);
				edited |= ImGui::SliderFloat2("scale", (float*)scale, 0.0f, 10.0f);

				Vector2 offset_delta = *offset - prev_offset;
				Vector2 scale_delta = *scale - prev_scale + Vector2One();
				if(edited) {
					for(auto it = selected_entities.begin(); it != selected_entities.end(); ++it) {
						Model* m = level.level_geo[*it]->model; 
						level_geo_t* g = level.level_geo[*it]; 
#if 1
						for(int i = 0; i < m->meshes->vertexCount; i++) {
							//x
							m->meshes->texcoords[i*2 + 0] *= scale_delta.x;
							m->meshes->texcoords[i*2 + 0] += offset_delta.x;
							//y
							m->meshes->texcoords[i*2 + 1] *= scale_delta.y;
							m->meshes->texcoords[i*2 + 1] += offset_delta.y;

							g->tex_offset = *offset;
							g->tex_scale = *scale;
						}
#else
						Vector3 cen = bb.min + (bb.max - bb.min)*0.5f;
						for(int i = 0; i < m->meshes->vertexCount; i++) {
							float tmin = FLT_MAX;
							Vector3 v = Vector3{ m->meshes->vertices[i*3 + 0], m->meshes->vertices[i*3 + 1], m->meshes->vertices[i*3 + 2] };
							Ray r = { cen, Vector3Normalize(v - cen) };

							plane_t p = compute_plane(UNIT_VECS[DIR_Y], bb.max);

							debug_drawer.add_ray(r, RED, 2000.f, false);

							Vector3 out_hit = Vector3Zero();
							bool hit = collision_ray_plane(r, p, &out_hit);
							if(hit) {
								if(Vector3Length(out_hit - cen) < tmin) {
									tmin = Vector3Length(out_hit - cen);
									debug_drawer.add_cross(out_hit, 0.5f, PURPLE, 200.0f, false);
									//convert intersection point to normalized uv
									Vector2 uv;
									uv.x = Normalize(out_hit.x, bb.min.x, bb.max.x);
									uv.y = Normalize(out_hit.z, bb.min.z, bb.max.z);
									m->meshes->texcoords[i*2 + 0] = uv.x;
									m->meshes->texcoords[i*2 + 1] = uv.y;
								}
							}
							p = compute_plane(UNIT_VECS[DIR_Z], bb.max);
							hit = collision_ray_plane(r, p, &out_hit);
							if(hit) {
								if(Vector3Length(out_hit - cen) < tmin) {
									tmin = Vector3Length(out_hit - cen);
									debug_drawer.add_cross(out_hit, 0.5f, PURPLE, 200.0f, false);
									//convert intersection point to normalized uv
									Vector2 uv;
									uv.x = Normalize(out_hit.x, bb.min.x, bb.max.x);
									uv.y = Normalize(out_hit.y, bb.min.y, bb.max.y);
									m->meshes->texcoords[i*2 + 0] = uv.x;
									m->meshes->texcoords[i*2 + 1] = uv.y;
								}
							}

							p = compute_plane(UNIT_VECS[DIR_X], bb.max);
							hit = collision_ray_plane(r, p, &out_hit);
							if(hit) {
								if(Vector3Length(out_hit - cen) < tmin) {
									tmin = Vector3Length(out_hit - cen);
									debug_drawer.add_cross(out_hit, 0.5f, PURPLE, 200.0f, false);
									//convert intersection point to normalized uv
									Vector2 uv;
									uv.x = Normalize(out_hit.y, bb.min.y, bb.max.y);
									uv.y = Normalize(out_hit.z, bb.min.z, bb.max.z);
									m->meshes->texcoords[i*2 + 0] = uv.x;
									m->meshes->texcoords[i*2 + 1] = uv.y;
								}
							}

							p = compute_plane(-UNIT_VECS[DIR_X], bb.min);
							hit = collision_ray_plane(r, p, &out_hit);
							if(hit) {
								if(Vector3Length(out_hit - cen) < tmin) {
									tmin = Vector3Length(out_hit - cen);
									debug_drawer.add_cross(out_hit, 0.5f, PURPLE, 200.0f, false);
									//convert intersection point to normalized uv
									Vector2 uv;
									uv.x = Normalize(out_hit.y, bb.max.y, bb.min.y);
									uv.y = Normalize(out_hit.z, bb.max.z, bb.min.z);
									m->meshes->texcoords[i*2 + 0] = uv.x;
									m->meshes->texcoords[i*2 + 1] = uv.y;
								}
							}


						}
#endif
						//rlUpdateMesh(m->meshes[0], 1, m->meshes->vertexCount);
					}
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Level")) {
				static bool first = true;
				static unsigned int num_levels = 0;
				static unsigned int selected_level = 0;
				static char* level_files[MAX_FILES_IN_DIR] = {};
				ImGui::Text("Filename: %s", level.level_filename);
				if(ImGui::Button("Save")) { level.save_level(level.level_filename); }
#if 1
				if(ImGui::Button("refresh level list") || first) {
					num_levels = list_of_files_in_dir(level_files, level_dir, ".txt");
					first = false;
				}

				ImGui::ListBox("", (int*)(&selected_level), level_files, num_levels, 10);
				ImGui::SameLine();
				if(ImGui::Button("Load")) {
					//level.saveLevel(level.file);
					level.empty();
					selected_entities.clear();
					level.load_level(level_files[selected_level]);
					level.compute_geo_bbs();
					reset();
				}
				ImGui::SameLine();
				if(ImGui::Button("New"))
				{
					if(cf_file_exists(level_dir "New.txt")) {
						ImGui::OpenPopup("newOverwrite");
					}
					else {
						level_t l;
						l.save_level(l.level_filename);
					}
				}
				if(ImGui::BeginPopup("newOverwrite")) {
					ImGui::Text("New.txt already exists.");
					ImGui::EndPopup();
				}
#endif
				ImGui::DragFloat3("Start Point", (float*)(&level.start_pos), 0.1f, 1000.0f, 1000.0f);
				if(ImGui::Button("Start here")) {
					level.start_pos = player.pos_m;
				}
				ImGui::DragFloat("Start Pitch", (float*)(&level.start_pitch), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Start Yaw", (float*)(&level.start_yaw), 0.01f, 0.0f, 6.2f);

				ImGui::Text("End Box");
				ImGui::DragFloat3("EMin", (float*)(&level.end_box.min), 0.1f, 1000.0f, 1000.0f);
				ImGui::DragFloat3("EMax", (float*)(&level.end_box.max), 0.1f, 1000.0f, 1000.0f);

				ImGui::DragFloat("fogLevelMin", &level.fogLevelMin, 1.0f, -5000.0f, 5000.0f);
				ImGui::DragFloat("fogLevelMax", &level.fogLevelMax, 1.0f, -5000.0f, 5000.0f);

				ImGui::Text("\nLights");
				const char* light_display[] = { "0", "1", "2", "3", "4" };
				ImGui::ListBox(" ", (int*)(&selected_light), light_display, 5, 5);
				ImGui::DragFloat3("Lights Pos", (float*)(&level.lights[selected_light].pos), 0.1f, -5000.0f, 5000.0f);
				ImGui::ColorEdit4("Light Col", (float*)&level.lights[selected_light].colour, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoAlpha);
				ImGui::DragFloat("Constant", (float*)(&level.lights[selected_light].constant), 0.1f, 0.0f, 500.0f);
				ImGui::DragFloat("Linear", (float*)(&level.lights[selected_light].linear), 0.001f, 0.0f, 1.0f, "%.4f");
				ImGui::DragFloat("Quadratic", (float*)(&level.lights[selected_light].quadratic), 0.0001f, 0.0f, 1.0f, "%.6f", 2.0f);
				ImGui::Checkbox("On", &level.lights[selected_light].on);

				if(ImGui::Button("Light here")) {
					level.lights[selected_light].pos = player.pos_m + player.look_dir*2.0f;
				}


				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("text trigger")) {
				static int selected_text_trigger = 0;
				static const char* arr[MAX_TEXT_TRIGGERS] = { 0 };
				char* poop = "0\0";
				for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
					arr[i] = poop;
				}
				for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
					text_trigger_t* t = &level.text_triggers[i];
					if(t->active) {
						if(i == selected_text_trigger) {
							debug_drawer.add_AABB(t->bb, BLUE, 0.0f, true);
						}
						else {
							debug_drawer.add_AABB(t->bb, RED, 0.0f, true);
						}
#if 0
						debug_drawer.add_cross(t->text_pos, 1.0f, BLUE, 0.0f, true);
#endif
					}
				}
				ImGui::ListBox("text triggers", &selected_text_trigger, arr, MAX_TEXT_TRIGGERS, 5);
				text_trigger_t* t = &level.text_triggers[selected_text_trigger];
				if(t->active) {
					t->displaying_text = true;
					t->disappear_timer.reset();
					Vector3 delta = { 0 };
					
					ImGui::DragFloat3("bb min ", (float*)&t->bb.min, 0.01f);
					ImGui::DragFloat3("bb max", (float*)&t->bb.max, 0.01f);
					ImGui::DragFloat3("spawn pos", (float*)&t->text_pos, 0.01f);
					ImGui::DragFloat3("move box", (float*)&delta, 0.01f);
					ImGui::DragFloat3("dir", (float*)&t->normal_dir, 0.1f);
					ImGui::DragFloat("angles", (float*)&t->angle, 0.1f);

					debug_drawer.add_vector(t->text_pos, level.text_triggers->normal_dir, 0.05f, 0.5f);
					debug_drawer.add_vector(t->text_pos, get_perp_vec(level.text_triggers->normal_dir), 0.05f, 0.5f, BLUE);
					if(ImGui::Button("wrap geo")) {
						convert_OBB_to_AABB(level.level_geo[*selected_entities.begin()]->OBB, &t->bb);
					}

					if(ImGui::Button("Put text at look dir")) {
						Ray r = { player_camera.position, player.look_dir };
						float out_t = 0;
						Vector3 out_q = Vector3Zero();
						collision_manager.cast_ray(r, OBJECT_TYPE::TYPE_LEVEL_GEO, &out_t, &out_q);
						t->text_pos = out_q;
					}
					static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
					ImGui::InputTextMultiline("text", t->str, MAX_TEXT_LENGTH, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

					t->bb.min += delta;
					t->bb.max += delta;
				}
				ImGui::Checkbox("active", &t->active);
				
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Enemy spawners")) {
				static int selected_spawner = 0;
				for(int i = 0; i < MAX_SPAWNERS; i++) {
					enemy_spawner_t* s = &level.spawners[i];
					if(s->active) {
						if(level.spawners[i].bb.min < level.spawners[i].bb.max) {
							if(i == selected_spawner) {
								debug_drawer.add_AABB(level.spawners[i].bb, BLUE, 0.0f, true);
							}
							else {
								debug_drawer.add_AABB(level.spawners[i].bb, RED, 0.0f, true);
							}
						}
#if 0
						debug_drawer.add_cross(level.spawners[i].pos_m, 1.0f, BLUE, 0.0f, true);
#endif
					}
				}
				static const char* arr[MAX_SPAWNERS] = { "0", "1", "2", "3", "4" };

				ImGui::ListBox("spawners", &selected_spawner, arr, MAX_SPAWNERS, 10);

				enemy_spawner_t* s = &level.spawners[selected_spawner];
				if(level.spawners[selected_spawner].active) {
					Vector3 delta = { 0 };
					ImGui::DragFloat3("bb min ", (float*)&s->bb.min, 0.1f);
					ImGui::DragFloat3("bb max", (float*)&s->bb.max, 0.1f);
					ImGui::DragFloat3("spawn pos", (float*)&s->pos_m, 0.1f);
					ImGui::Text("Enemies in spawner");
					ImGui::DragScalar("Chaser", ImGuiDataType_U8, &s->num_enemy[(int)ENEMY_TYPE::ENEMY_CHASER], 0.1f);
					ImGui::DragScalar("Shooter", ImGuiDataType_U8, &s->num_enemy[(int)ENEMY_TYPE::ENEMY_SHOOTER], 0.1f);
					ImGui::DragScalar("Flyer", ImGuiDataType_U8, &s->num_enemy[(int)ENEMY_TYPE::ENEMY_FLYER], 0.1f);
					ImGui::DragScalar("Spawner", ImGuiDataType_U8, &s->num_enemy[(int)ENEMY_TYPE::ENEMY_SPAWNER], 0.1f);
					ImGui::DragScalar("Buffer", ImGuiDataType_U8, &s->num_enemy[(int)ENEMY_TYPE::ENEMY_BUFFER], 0.1f);
					ImGui::DragFloat3("move all", (float*)&delta, 0.1f);

					s->bb.min += delta;
					s->bb.max += delta;
					s->pos_m += delta;

				}
				if(ImGui::Button("Init spawner")) { level.init_spawner(selected_spawner, player.pos_m + player.look_dir*5); }
				
				if(level.spawners[selected_spawner].active) {
					if(ImGui::Button("delete spawner")) { level.spawners[selected_spawner].deactivate(); }
				}

				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Options/Perf")) {
				ImGui::SliderFloat("Editor move speed", &player.editor_acel_mult, 1.0f, 20.0f);
				ImGui::SliderFloat("music vol", &audio_manager.music_vol, 0.0f, 1.0f);
				ImGui::SliderFloat("sound vol", &audio_manager.sound_vol, 0.0f, 1.0f);
				ImGui::Checkbox("Editor noclip", &player.editor_noclip);
				ImGui::Text("Debug 3D prim array size: %i", debug_drawer.drawables.size());
				ImGui::Checkbox("Debug collision detection", &debug_collision_detection);
				ImGui::Checkbox("Debug enemy", &debug_enemy);
				ImGui::Text("Debug collision detection:\n"
					        "BLUE = collidable AABB\n"
					        "ORANGE = possible collision, passed col filter\n"
							"RED = real prims are colliding");
				ImGui::DragFloat("fixup_x", &player.fixup_x, 0.01f);
				ImGui::DragFloat("fixup_y", &player.fixup_y, 0.01f);
				ImGui::DragFloat("fixup_z", &player.fixup_z, 0.01f);
				ImGui::DragFloat3("pos_fixup", (float*)&(player.fixup_pos), 0.01f);
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
				//ImGui::Checkbox("Camera roll", &player.cameraRoll);
				float FOVMin = 90.0f;
				float FOVMax = 120.0f;
				//ImGui::SliderScalar("FOV", ImGuiDataType_Float, &player.camera.fovY, &FOVMin, &FOVMax);
				unsigned int skyMin = 1;
				unsigned int skyMax = 8;
				//ImGui::Text("Frame time: %lu", elapsed);
				//ImGui::Text("Frame rate: %.2f", 1.0f/((float) (elapsed)/1000000.0f));
				ImGui::EndTabItem();
			}
#if ATGM_TESTING
			if (ImGui::BeginTabItem("ATGM testing")) {

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2);

	 ImGui::DragFloat("speed", &atgm.speed_mps, 0.01f);
	 ImGui::DragInt("eval_num", &atgm.eval_num);
	 ImGui::DragFloat("eval per second", &atgm.eval_ps, 0.01f);

	 ImGui::Dummy(ImVec2(0.0f, 20.0f));

	 ImGui::DragFloat("jitter", &atgm.jitter, 0.01f);
	 ImGui::DragFloat("jitter dist scale", &atgm.jitter_dist_scale, 0.01f);

	 ImGui::Dummy(ImVec2(0.0f, 20.0f));

	 ImGui::DragFloat("side to side rad", &atgm.side_to_side_radius_m, 0.01f);
	 ImGui::DragFloat("side to side radius dist scale", &atgm.side_to_side_radius_dist_scale, 0.01f);
	 ImGui::DragFloat("side to side period", &atgm.side_to_side_period_m, 0.01f);
	 ImGui::DragFloat("side to side period scale", &atgm.side_to_side_period_scale, 0.01f);

	 ImGui::Dummy(ImVec2(0.0f, 20.0f));

	 ImGui::DragFloat("up down rad", &atgm.up_down_radius_m, 0.01f);
	 ImGui::DragFloat("up down rad dist scale", &atgm.up_down_radius_dist_scale, 0.01f);
	 ImGui::DragFloat("up down period", &atgm.up_down_period_m, 0.01f);
	 ImGui::DragFloat("up down period scale", &atgm.up_down_period_scale, 0.01f);

	 ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}
#endif
			ImGui::EndTabBar();

			ImGui::End();

			for(unsigned int i = 0; i < level.level_geo.size(); i++)
			{
				delete listbox_display[i];
			}
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

	BeginMode3D(player_camera);

	rlDisableBackfaceCulling();
	rlDisableDepthMask();
	    DrawModel(skybox, player_camera.position, 1.0f, WHITE);
	    //DrawModel(skybox, Vector3Zero(), 1.0f, WHITE);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();

	int drawing_far_plane = 0;

#define X(shader, name, address, type)\
SetShaderValue(shader, u_ ## name ## _loc, address, type);
	UNIFORMS
#undef X


	//set light uniforms
	for(int i = 0; i < NUM_LIGHTS; i++) {
		SetShaderValue(ground_shader, level.lights[i].u_pos_loc, &level.lights[i].pos, SHADER_UNIFORM_VEC3);
		SetShaderValue(ground_shader, level.lights[i].u_col_loc, &level.lights[i].colour, SHADER_UNIFORM_VEC3);
		SetShaderValue(ground_shader, level.lights[i].u_constant_loc, &level.lights[i].constant, SHADER_UNIFORM_FLOAT);
		SetShaderValue(ground_shader, level.lights[i].u_linear_loc, &level.lights[i].linear, SHADER_UNIFORM_FLOAT);
		SetShaderValue(ground_shader, level.lights[i].u_quadratic_loc, &level.lights[i].quadratic, SHADER_UNIFORM_FLOAT);
		SetShaderValue(ground_shader, level.lights[i].u_on_loc, &level.lights[i].on, SHADER_UNIFORM_INT);
	}

	//Draw entities
	for(int i = 0; i < level.level_geo.size(); i++) {
		//DrawModelEx(*level.entities[i]->model, level.entities[i]->pos,	level.entities[i]->rotAxis, level.entities[i]->angle, level.entities	[i]->scale, BLUE);
		DrawModelEx(*level.level_geo[i]->model, level.level_geo[i]->pos_m, level.level_geo[i]->rot_axis, level.level_geo[i]->angle*RAD2DEG, level.level_geo[i]->scale, WHITE);
	}
#if 0
	drawing_far_plane = 1;
	SetShaderValue(ground_shader, u_drawing_far_plane_loc, &drawing_far_plane, SHADER_UNIFORM_INT);
	rlPushMatrix();
	Vector3 pos = player_camera.position + player.look_dir*(RL_CULL_DISTANCE_FAR/2);
	rlTranslatef(pos.x, pos.y, pos.z);
	rlRotatef(player.cam_yaw_r*RAD2DEG, 0, 1, 0);
	rlRotatef(-player.cam_pitch_r*RAD2DEG - 90.0f, 1, 0, 0);
	//rlRotatef(-player.cam_roll_r*RAD2DEG, 0, 0, 1);
	BeginShaderMode(ground_shader);

	DrawPlane(Vector3Zero(), Vector2{ 20000, 20000 }, WHITE);

	EndShaderMode();
	rlPopMatrix();
#endif

	drawing_far_plane = 0;
	SetShaderValue(ground_shader, u_drawing_far_plane_loc, &drawing_far_plane, SHADER_UNIFORM_INT);

	//Draw enemies
	if(!editor_mode) {
		enemy_manager.draw_enemies(extrap_dt_s);
	}

	atgm.draw();

	//Draw bullets
	for(int i = 0; i < MAX_BULLETS; i++) {
		bullet_t* b = &bullet_manager.bullets[i];
		if(b->active) {
			if(b->friendly) {
				DrawSphereEx(b->pos_m + b->velocity_mps*extrap_dt_s, b->radius_m, 5, 5, GREEN);
//				DrawSphereEx(b->pos_m, b->shield_radius_m, 5, 5, Color{ 0, 255,0, 100 });
			}
			else {
				DrawSphereEx(b->pos_m + b->velocity_mps*extrap_dt_s, b->radius_m, 5, 5, RED);
			}
		}
	}

	particle_system.draw_particles(player_camera);
	level.draw_text_triggers_3d();
	player.draw();

	static bool finish = false;
	char str[] = "YOU ENDED THE POINTLESS SUFFERING\nOF: %i/%i BEINGS IN THIS ZONE. FINISH.";
	char ummm[1000];
	sprintf(ummm, str, killed_enemies, level.total_enemies);
	if(player.pos_m.z < -1272 || finish) {
	//DrawText3D(GetFontDefault(), ummm, Vector3{-139, 151, -1402}, UNIT_VECS[DIR_Y], 180, 100.0f, 1.0f, 1.0f, true, RED);
	finish = true;
	}

	debug_drawer.draw_debug();

	EndMode3D();

	player.draw_hud();

	level.draw_text_triggers_2d();
	debug_drawer.draw_debug_text();
	debug_drawer.update_drawables(game_dt);

	DrawFPS(10, 10);

	imgui();

	EndDrawing();
}

#if 0
//Draw sky
float sky[] = {
100, 1,  100,
100, 1, -100,
-100, 1, -100,
-100, 1,  100
};
unsigned int skyindices[] = {
0, 1, 3, // first triangle
1, 2, 3  // second triangle
};

glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glDepthFunc(GL_LEQUAL);
glm::mat4 model = glm::mat4();
glm::mat4 view = glm::mat4(glm::mat3(player.camera.viewMat));
skySdr.use();
skySdr.setMat4("model", model);
skySdr.setMat4("view", view);
skySdr.setMat4("projection", projection);
skySdr.setFloat("u_time", uptime.getElapsedTime().asSeconds());

skySdr.setVec2("resolution", glm::vec2(10, 10));
skySdr.setVec3("clearColour", level.clearColour);
skySdr.setVec3("skyColour1", level.skyCol1);
skySdr.setVec3("skyColour2", level.skyCol2);
skySdr.setVec3("skyColour3", level.skyCol3);
skySdr.setVec3("skyColour4", level.skyCol4);
skySdr.setInt("NUM_OCTAVES", skyOctaves);
unsigned int skyVBO, skyVAO, skyEBO;

glGenVertexArrays(1, &skyVAO);
glGenBuffers(1, &skyVBO);
glGenBuffers(1, &skyEBO);

glBindVertexArray(skyVAO);

glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(sky), sky, GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyindices), skyindices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glBindVertexArray(skyVAO);
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

//Cleanup
glBindVertexArray(0);
glDeleteVertexArrays(1, &skyVAO);
glDeleteBuffers(1, &skyVBO);
glDeleteBuffers(1, &skyEBO);
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glDepthFunc(GL_LESS);
#endif

void load_shaders_and_font() {
	ground_shader = LoadShader(shader_dir "base_lighting.vs", shader_dir "shader.fs");

	ground_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(ground_shader, "matModel");

#define X(shader, name, address, type)\
u_ ## name ## _loc = GetShaderLocation(shader, "u_" #name "\0" );
	UNIFORMS
#undef X

	//light locations
	for(int i = 0; i < NUM_LIGHTS; i++) {
		char pos_name[32] = "u_lights[x].pos\0";
		char col_name[32] = "u_lights[x].colour\0";
		char constant_name[32] = "u_lights[x].constant\0";
		char linear_name[32] = "u_lights[x].linear\0";
		char quadratic_name[32] = "u_lights[x].quadratic\0";
		char on_name[32] = "u_lights[x].on\0";

		pos_name[9] = '0' + i;
		col_name[9] = '0' + i;
		constant_name[9] = '0' + i;
		linear_name[9] = '0' + i;
		quadratic_name[9] = '0' + i;
		on_name[9] = '0' + i;

		level.lights[i].u_pos_loc = GetShaderLocation(ground_shader, pos_name);
		level.lights[i].u_col_loc = GetShaderLocation(ground_shader, col_name);
		level.lights[i].u_constant_loc = GetShaderLocation(ground_shader, constant_name);
		level.lights[i].u_linear_loc = GetShaderLocation(ground_shader, linear_name);
		level.lights[i].u_quadratic_loc = GetShaderLocation(ground_shader, quadratic_name);
		level.lights[i].u_on_loc = GetShaderLocation(ground_shader, on_name);
	}


    // Load skybox model
    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    skybox = LoadModelFromMesh(cube);
	Image img = LoadImage(asset_dir "night.png");
	skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
	skybox.materials[0].shader = LoadShader(shader_dir "skybox.vs", shader_dir "skybox.fs");

	int hmm = MATERIAL_MAP_CUBEMAP;
	SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), &hmm, SHADER_UNIFORM_INT);
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

	//@TEST:testing pitch adjusting
	audio_manager.init();

	load_shaders_and_font();

	level.load_level("New.txt");
	//level.startPos = Vector3Zero();
	particle_system.init(asset_dir "particle_tex.png");
	level.compute_geo_bbs();

	collision_manager.on_overlap = my_on_overlap;

	player.init();

	reset();

	SetCameraMode(player_camera, CAMERA_PERSPECTIVE);

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
		lag_s = min(1.0f, lag_s);
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

