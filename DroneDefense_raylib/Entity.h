#pragma once
#include "pch.h"

#include "common.h"
#include "debug_draw.h"
#include "collision_manager.h"
#include "audio_manager.h"
#include "particle.h"

using namespace std;

struct light_t {
	Vector3 pos = Vector3Zero();
	Vector3 colour = Vector3One();
	float constant = 1.0f;
	float linear = 0.0007f;
	float quadratic = 0.00001f;
	bool on = true;

	// Shader locations
    unsigned int u_pos_loc;
    unsigned int u_col_loc;
    unsigned int u_constant_loc;
    unsigned int u_linear_loc;
    unsigned int u_quadratic_loc;
    unsigned int u_on_loc;
};

#define MAX_TEXT_LENGTH 1024 
struct text_trigger_t {
	bool active = false;
	bool displaying_text = false;
	BoundingBox bb = { 0 };
	Vector3 text_pos = Vector3Zero();
	Vector3 normal_dir = UNIT_VECS[DIR_Y];
	float angle = 0.0f;
	char str[MAX_TEXT_LENGTH] = { 0 };
	timer_t disappear_timer = { 0.0f, 1.0f };
};

struct level_geo_t {
	int ID = -1;
	Model* model = nullptr;
	char model_file_name[CUTE_FILES_MAX_FILENAME] = { 0 };
	char tex_file_name[CUTE_FILES_MAX_FILENAME] = { 0 };
	Vector2 tex_scale = Vector2One();
	Vector2 tex_offset = Vector2Zero();

	OBB_t OBB = { 0 };
	BoundingBox bb = { 0 };
	Vector3 pos_m = Vector3Zero();
	Vector3 scale = Vector3One();
	Vector3 rot_axis = UNIT_VECS[DIR_Z];
	float angle = 0.0f; //radians!
	bool collision = true;

	void init(){}

	void destroy(){}

	void compute_bounding_box() {
		//we first compute a straight up AABB 
		BoundingBox bounding_box = GetMeshBoundingBox(model->meshes[0]);

		OBB_t temp_obb;
		//convert aabb to obb
		convert_AABB_to_OBB(bounding_box, &temp_obb);

		Vector3 pts[8];
		convert_OBB_to_polygon(temp_obb, &pts);
		// Calculate transformation matrix from function parameters
		// Get transform matrix (rotation -> scale -> translation)
		Matrix mat_scale = MatrixScale(scale.x, scale.y, scale.z);
		Matrix mat_rotation = MatrixRotate(rot_axis, angle);
		Matrix mat_translation = MatrixTranslate(pos_m.x, pos_m.y, pos_m.z);

		Matrix mat_transform = MatrixMultiply(MatrixMultiply(mat_scale, mat_rotation), mat_translation);

		// Combine model transformation matrix (model.transform) with matrix generated by function parameters (matTransform)
		mat_transform = MatrixMultiply(model->transform, mat_transform);

		for(int i = 0; i < 8; i++) {
			pts[i] = Vector3Transform(pts[i], mat_transform);
		}

		convert_polygon_to_OBB(&pts, &OBB);
		convert_OBB_to_AABB(OBB, &bb);
	}
};

struct bullet_t {
	bool active = false;
	
	Vector3 pos_m = Vector3{ NAN, NAN, NAN };
	Vector3 velocity_mps = Vector3{ NAN, NAN, NAN };

	float radius_m = 0.5f;
	float shield_radius_m = 0.0f;
	int damage = 0;
	bool friendly = false;

	void init(Vector3 start_pos, Vector3 in_vel, float in_rad, float in_shield_rad, int in_damage, bool in_friendly) {
		pos_m = start_pos;
		velocity_mps = in_vel;
		radius_m = in_rad;
		shield_radius_m = in_shield_rad;
		damage = in_damage;
		friendly = in_friendly;

		active = true;
	}

	void deactivate() {
		pos_m = Vector3{ NAN, NAN, NAN };
		velocity_mps = Vector3Zero();
		active = false;
	}

	void update() {
		pos_m = pos_m + velocity_mps*dt_s;
	}
};

//@TODO: add function to destroy bullets that go too far away/exist for too long
struct bullet_manager_t {
	unsigned int bullets_i = 0;
	bullet_t bullets[MAX_BULLETS];

	void add_bullet(Vector3 start, Vector3 dir, float speed_mps, float radius_m, float sheild_radius_m, int damage, bool f) {
		bool activation_success = false;
		for(int i = bullets_i; i != (bullets_i - 1 + MAX_BULLETS) % MAX_BULLETS; i = (i + 1) % MAX_BULLETS) {
			bullet_t* b = &bullets[bullets_i];
			if(!b->active) {
				b->init(start, dir*speed_mps, radius_m, sheild_radius_m, damage, f);
				activation_success = true;
				bullets_i = (bullets_i + 1) % MAX_BULLETS;
				break;
			}
		}
		
		//if we did not activate a bullet in the above, this means the array is full and we overwrote the oldest bullet
		if(!activation_success) {
			bullet_t* b = &bullets[bullets_i];
			b->init(start, dir*speed_mps, radius_m, sheild_radius_m, damage, f);
			bullets_i = (bullets_i + 1) % MAX_BULLETS;
#if _DEBUG
			//printf("WARNING: Bullet activation was not sucessful, we overwrote the oldest bullet, try increasing MAX_BULLETS.\n");
#endif
		}
	}
};

enum class ENEMY_TYPE {
	ENEMY_CHASER,
	ENEMY_SHOOTER,
	ENEMY_FLYER,
	ENEMY_SPAWNER,
	ENEMY_BUFFER,
	ENEMY_MAX,
	ENEMY_MIN = 0
};

void add_enemy(Vector3, ENEMY_TYPE);

struct enemy_t {
	ENEMY_TYPE type = ENEMY_TYPE::ENEMY_MIN;
	Vector3 pos_m = Vector3{ NAN, NAN, NAN };
	Vector3 velocity_mps;
	Vector3 acel_mps2 = Vector3Zero();
	Vector3 dims_m = Vector3One(); //full length dims of char
	BoundingBox bounding_box;

	float distance_until_attack = 2.0f;
	float melee_bounding_box_height = 1.5f;
	float melee_bounding_box_width = 3.0f;
	BoundingBox melee_bounding_box; //bounds do not change
	float melee_bounding_box_offest = 3.0f;
	timer_t melee_wind_up_timer;
	bool in_melee_windup = false;
	bool melee_attack = false;
	bool melee_hurtbox_active = false;

	float move_acel_mps2 = 100.0f;
	float base_max_speed_mps = 10.0f;
	bool on_ground = false;

	int max_health = -1;
	int health = -1;
	int melee_damage = 20;

	float ability_timer_base_period_s = 1.0f;
	timer_t ability_timer = { 0 };

	uint8_t buff_level = 0;
	const uint8_t max_buff_level = 3;

	bool can_use_ability = false;

	//speed = base_speed + speed_buff_mps*buff_level
	float speed_buff_mps = 0.0f;

	//ability_timer.freq = 1.0f/(ability_timer_base_period_s -action_buff_period_s*buff_level);
	float ability_period_buff_s = 0.0f;

	timer_t noise_timer = { 0 };
	SOUNDS sound = SOUNDS::zombie3;

	//shooter
	int gun_damage = 10;
	float shoot_speed_mps = 2.0f;
	float bullet_radius_m = 0.5f;

	//buffer
	float buff_radius_m = 20.0f;

	Vector3 offset;

	void construct_bounding_box() {
		bounding_box.min = pos_m - dims_m*0.5f;
		bounding_box.max = pos_m + dims_m*0.5f;
	}

	void init(Vector3 pos_in, ENEMY_TYPE t) {
		type = t;
		pos_m = pos_in;

		switch(type)
		{
		case ENEMY_TYPE::ENEMY_CHASER:
			offset = Vector3{ rand_ndc()*5.0f, 0, rand_ndc()*5.0f };
			base_max_speed_mps = 16.0f;
			max_health = 20;
			ability_timer_base_period_s = 999999.f;
			dims_m = Vector3{0.5f, 1.5f, 0.5f};
			melee_damage = 15;
			speed_buff_mps = 2.0f;
			break;
		case ENEMY_TYPE::ENEMY_SHOOTER:
			offset = Vector3{ rand_ndc()*20.0f, 0, rand_ndc()*20.0f };
			base_max_speed_mps = 12.0f;
			move_acel_mps2 = 20.0f;
			max_health = 10;
			ability_timer_base_period_s = 6.0f;
			dims_m = Vector3{0.5f, 1.5f, 0.5f};
			melee_damage = 7;
			ability_period_buff_s = 0.5f;
			break;
		case ENEMY_TYPE::ENEMY_FLYER:
			offset = Vector3{ rand_ndc()*1.0f, rand_ndc()*1.0f, rand_ndc()*1.0f };
			base_max_speed_mps = 15.0f;
			move_acel_mps2 = 50.0f;
			max_health = 5;
			ability_timer_base_period_s = 999999.f;
			dims_m = Vector3{0.8f, 0.8f, 0.8f};
			melee_damage = 3;
			speed_buff_mps = 2.5f;
			melee_bounding_box_width = 1.0f;
			melee_bounding_box_height = 1.0f;
			break;
		case ENEMY_TYPE::ENEMY_SPAWNER:
			offset = Vector3{ rand_ndc()*10.0f, 0, rand_ndc()*10.0f };
			base_max_speed_mps = 7.0f;
			max_health = 50;
			ability_timer_base_period_s = 5.0f;
			dims_m = Vector3{1.5f, 2.5f, 1.5f};
			melee_damage = 10;
			ability_period_buff_s = 1.0f;
			break;
		case ENEMY_TYPE::ENEMY_BUFFER:
			offset = Vector3{ rand_ndc()*10.0f, 0, rand_ndc()*10.0f };
			base_max_speed_mps = 10.0f;
			max_health = 30;
			ability_timer_base_period_s = 30.0f;
			dims_m = Vector3{1.0f, 2.0f, 1.0f};
			melee_damage = 10;
			break;
		default:
			//Unhandled enemy type
			assert(false);
			break;
		}

		buff_level = 0;

		health = max_health;
		construct_bounding_box();
 
		melee_wind_up_timer.freq_hz = 1.0f/0.4f;
		melee_wind_up_timer.accum_s = -1.0f;

		ability_timer.freq_hz = 1.0f/ability_timer_base_period_s;
		ability_timer.accum_s = ability_timer_base_period_s + rand_norm()*ability_timer_base_period_s;
	}

	void deactivate() {
		type = ENEMY_TYPE::ENEMY_MIN;
		pos_m = Vector3{ NAN, NAN, NAN };
		health = -1;
	}

	BoundingBox construct_melee_bb(Vector3 offset) {
		BoundingBox bb;
		bb.min = Vector3{ -melee_bounding_box_width/2, -melee_bounding_box_height/2, -melee_bounding_box_width/2 };
		bb.max = Vector3{ melee_bounding_box_width/2, melee_bounding_box_height/2, melee_bounding_box_width/2 };
		bb.min += offset;
		bb.max += offset;
		return bb;
	}

	void update(Vector3 enemy_pos) {
		can_use_ability = ability_timer.tick(dt_s) && health > 0;
		if(in_melee_windup) {
			melee_attack = melee_wind_up_timer.tick(dt_s) && health > 0;
		}

		melee_hurtbox_active = false;
		on_ground = false;

		if(can_use_ability) {
			switch(type)
			{
			case ENEMY_TYPE::ENEMY_CHASER:
				break;
			case ENEMY_TYPE::ENEMY_SHOOTER:
				bullet_manager.add_bullet(pos_m, enemy_pos - pos_m, shoot_speed_mps, bullet_radius_m, 0.0f, gun_damage, false);
				audio_manager.add_sound_fx_positional(SOUNDS::poison, 0.1f, pos_m, true);
				break;
			case ENEMY_TYPE::ENEMY_FLYER:
				break;
			case ENEMY_TYPE::ENEMY_SPAWNER:
				add_enemy(pos_m, ENEMY_TYPE::ENEMY_FLYER);
				audio_manager.add_sound_fx_positional(SOUNDS::spawn, 1.0f, pos_m, true);
				break;
			case ENEMY_TYPE::ENEMY_BUFFER:
				audio_manager.add_sound_fx_positional(SOUNDS::Down1, 2.0f, pos_m, true);
				break;
			}
			ability_timer.reset();
		}

#if 0
		Ray r = { pos_m, -UNIT_VECS[DIR_Y] };
		float dist;
		if(collision_manager.cast_ray(r, COL_LEVEL_GEO, &dist) && dist < 1.0f) {
			acel_mps2.y = 0;
		}
		else {
			acel_mps2.y -= 1.0f;
		}

#endif
		if(type != ENEMY_TYPE::ENEMY_FLYER && !on_ground) {
			acel_mps2.y = -9.81f;
		}

		Vector3 distance_from_enemy = enemy_pos - pos_m;
		Vector3 enemy_dir = Vector3Normalize(distance_from_enemy);

		Vector3 distance_from_goal_pos = enemy_pos + offset - pos_m;
		Vector3 goal_pos_dir = Vector3Normalize(distance_from_goal_pos);

		if(Vector3Length(distance_from_enemy) < distance_until_attack && !in_melee_windup && !melee_attack) {
			in_melee_windup = true;
			melee_wind_up_timer.reset();
		}

		if(in_melee_windup) {
			melee_bounding_box = construct_melee_bb(pos_m + enemy_dir*melee_bounding_box_offest);

		}

		if(melee_attack) {
			melee_attack = false;
			in_melee_windup = false;
			melee_hurtbox_active = true;
			melee_wind_up_timer.accum_s = -1;
			melee_bounding_box = construct_melee_bb(pos_m + enemy_dir*melee_bounding_box_offest);
			//debug_drawer.add_AABB(melee_bounding_box, RED, 0.5f, true);
		}

		if(type != ENEMY_TYPE::ENEMY_FLYER) {
			goal_pos_dir.y = 0;
		}

		const float max_speed_mps = base_max_speed_mps + speed_buff_mps*buff_level;
		Vector3 moving_vel_mps = goal_pos_dir*move_acel_mps2*dt_s + velocity_mps;
		Vector3 just_planer_moving_vel_mps = moving_vel_mps;
		const float	temp_y = moving_vel_mps.y;
		if(type != ENEMY_TYPE::ENEMY_FLYER) {
			just_planer_moving_vel_mps.y = 0;
		}

		if(Vector3Length(just_planer_moving_vel_mps) > max_speed_mps) {
			just_planer_moving_vel_mps = Vector3Normalize(just_planer_moving_vel_mps)*max_speed_mps;
		}
		just_planer_moving_vel_mps.y = temp_y;

		velocity_mps = just_planer_moving_vel_mps + acel_mps2*dt_s;

		if(!in_melee_windup) {
			pos_m += velocity_mps*dt_s;
		}
		construct_bounding_box();

		if(debug_enemy) {
#if 0
			debug_drawer.add_cross(enemy_pos + offset, 1.0f, BLUE);
#endif
			//debug_drawer.add_vector(pos_m, velocity_mps, 0.1f, 0.1f);
		}
	}

	void take_damage(int damage) {
		health -= damage;
		if(health <= 0) {
			deactivate();
			if(type != ENEMY_TYPE::ENEMY_FLYER) {
				killed_enemies++;
			}
		}
	}

	void apply_buff() {
		buff_level = std::min((uint8_t)(buff_level + 1), max_buff_level);
		ability_timer.freq_hz = 1.0f/(ability_timer_base_period_s - ability_period_buff_s*buff_level);
	}
};

//@Cleanup: this should be in the enemy manager but the enemy needs to know 
//about it to spawn itself.
void add_enemy(Vector3 pos_in, ENEMY_TYPE t) {
	bool spawn_success = false;
	for(int i = 0; i < MAX_ENEMIES; i++) {
		enemy_t* e = &enemies[i];
		if(e->health <= 0) {
			e->init(pos_in, t);
			spawn_success = true;
			break;
		}
	}
	//Could not spawn enemy
	assert(spawn_success);
}

struct enemy_manager_t {
	void kill_all_enemies() {
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			e->deactivate();
		}
	}

	void update_enemies(Vector3 goal_pos) {
		static timer_t buff_particle_timer = { 0, 2.0f };
		bool spawn_buff_particles = buff_particle_timer.tick(dt_s);
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			if(e->health > 0) {
				e->update(goal_pos);
				if(spawn_buff_particles) {
					particle_system.activate_particles(e->buff_level, e->pos_m + UNIT_VECS[DIR_Y]*e->dims_m.y*0.5f, UNIT_VECS[DIR_Y], WHITE, 1.0f, false);
				}
			}
		}

		if(spawn_buff_particles) {
			buff_particle_timer.reset();
		}
	}

	void draw_enemies(float extrap_dt_s) {
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			if(e->health > 0.0f) {
				Color c = RED;
				switch(e->type)
				{
				case ENEMY_TYPE::ENEMY_CHASER: {
					c = ORANGE;
					break;
				}
				case ENEMY_TYPE::ENEMY_FLYER: {
					c = BLUE;
					break;
				}
				case ENEMY_TYPE::ENEMY_SHOOTER: {
					c = PURPLE;
					break;
				}
				case ENEMY_TYPE::ENEMY_SPAWNER: {
					c = BLACK;
					break;
				}
				case ENEMY_TYPE::ENEMY_BUFFER: {
					c = WHITE;
					break;
				}
				default:
					assert(false);
					break;
				}

				if(e->in_melee_windup) {
					Vector3 dims = e->melee_bounding_box.max - e->melee_bounding_box.min;
				Vector3 pos = e->melee_bounding_box.min + dims*0.5f;
				Color melee_c = lerp(GREEN, RED, 1 - e->melee_wind_up_timer.accum_s/(1.0f/e->melee_wind_up_timer.freq_hz));
					DrawCubeWiresV(pos, dims, melee_c);
				}

				DrawCube(e->pos_m, e->dims_m.x, e->dims_m.y, e->dims_m.z, lerp(RED, c, (float)e->health/(float)e->max_health));
		//		DrawCube(e->pos_m + UNIT_VECS[DIR_Y]*(e->dims_m.y + 0.05f) + e->velocity_mps*extrap_dt_s, 0.1f, 0.1f, 0.1f, lerp(RED, GREEN, e->ability_timer.accum_s/(1/e->ability_timer.freq_hz)));
			}
		}
	}

	void add_collidables() {
		//enemy
		uint16_t enemy_collides_with =  COL_PLAYER_MELEE | COL_FRIEND_BULLET | COL_PLAYER | COL_ENEMY | COL_LEVEL_GEO | COL_ENEMY_BUFF_VOL;
		primitive_t enem_prim;
		enem_prim.prim_type = PRIM_TYPE::PRIM_AABB;
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			if(e->health > 0) {
				enem_prim.prim.aabb = e->bounding_box;
				collision_manager.add_collidable(e, e->bounding_box, OBJECT_TYPE::TYPE_ENEMY, enem_prim, COL_ENEMY, enemy_collides_with);
			}
		}

		//enemy melee
		uint16_t enemy_melee_collides_with = COL_PLAYER;
		primitive_t en_melee_prim;
		en_melee_prim.prim_type = PRIM_TYPE::PRIM_AABB;
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			if(e->melee_hurtbox_active) {
				BoundingBox b = e->melee_bounding_box;
				en_melee_prim.prim.aabb = b;
				collision_manager.add_collidable(e, b, OBJECT_TYPE::TYPE_ENEMY, en_melee_prim, COL_ENEMY_MELEE, enemy_melee_collides_with);
			}
		}

		//enemy buff volume
		uint16_t buff_volume_collides_with = COL_ENEMY;
		primitive_t buff_volume;
		buff_volume.prim_type = PRIM_TYPE::PRIM_SPHERE;
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_t* e = &enemies[i];
			if(e->can_use_ability && e->type == ENEMY_TYPE::ENEMY_BUFFER) {
				BoundingBox buff_bb;
				buff_bb.min = e->pos_m - Vector3One()*e->buff_radius_m;
				buff_bb.max = e->pos_m + Vector3One()*e->buff_radius_m;
				buff_volume.prim.sph.c = e->pos_m;
				buff_volume.prim.sph.r = e->buff_radius_m;

				collision_manager.add_collidable(e, buff_bb, OBJECT_TYPE::TYPE_ENEMY_BUFF_VOL, buff_volume, COL_ENEMY_BUFF_VOL, buff_volume_collides_with);
				particle_system.activate_particles_from_point_radius(50, e->pos_m, 10, WHITE, 1.0f);
			}
		}
	}
};

struct enemy_spawner_t {
	//If this spawner has been init
	bool active = false;
	//if this spawner has spawned its enemies
	bool spawned = false;
	Vector3 pos_m = Vector3Zero();
	BoundingBox bb = { 0 };
	uint8_t num_enemy[(int)ENEMY_TYPE::ENEMY_MAX];

	void init(const int arr[(int)ENEMY_TYPE::ENEMY_MAX], Vector3 in_pos, BoundingBox in_bb = {Vector3Zero(), Vector3One()}) {
		for(int i = (int)ENEMY_TYPE::ENEMY_MIN; i < (int)ENEMY_TYPE::ENEMY_MAX; i++) {
			num_enemy[i] = arr[i];
		}
		pos_m = in_pos;
		bb = in_bb;
		active = true;
	}

	void deactivate() {
		active = false;
		pos_m = Vector3Zero();
		bb = { 0 };
	}

	void reset() {
		if(active) {
			spawned = false;
		}
	}

	void spawn() {
		if(!spawned) {
			for(int i = (int)ENEMY_TYPE::ENEMY_MIN; i < (int)ENEMY_TYPE::ENEMY_MAX; i++) {
				for(int j = 0; j < num_enemy[i]; j++) {
					add_enemy(pos_m, (ENEMY_TYPE)i);
				}
			}
			spawned = true;
		}
	}
};

enum PLAYERSTATE { AIR, GROUND, WALL };

// Returns xyz-rotation matrix (angles in radians)
Matrix MatrixRotateXYZ_old(Vector3 ang)
{
	Matrix result = MatrixIdentity();

	float cosz = cosf(-ang.z);
	float sinz = sinf(-ang.z);
	float cosy = cosf(-ang.y);
	float siny = sinf(-ang.y);
	float cosx = cosf(-ang.x);
	float sinx = sinf(-ang.x);

	result.m0 = cosz * cosy;
	result.m4 = (cosz * siny * sinx) - (sinz * cosx);
	result.m8 = (cosz * siny * cosx) + (sinz * sinx);

	result.m1 = sinz * cosy;
	result.m5 = (sinz * siny * sinx) + (cosz * cosx);
	result.m9 = (sinz * siny * cosx) - (cosz * sinx);

	result.m2 = -siny;
	result.m6 = cosy * sinx;
	result.m10 = cosy * cosx;

	return result;
}

struct player_t {
	int max_health = 100;
	int health = max_health;
	timer_t invuln_timer;

	BoundingBox bounding_box;
	BoundingBox bottom_bounding_box; //used to make stairs work and see that we are on ground
	float downward_collision_margin_m = 0.005f;
	// angle of colliding ground normal to be considered ground
	const float ground_angle_deg = 45.0f; 
	float max_stepable_stair_height_m = 0.5f;
	const float height_m = 2.0f;
	const float width_m = 0.8f;
	const float eye_height_m = 0.9f*height_m;
	bool camera_roll = true;
	Vector3 look_dir;

	//radians
	float cam_pitch_r = 0.0f;
	float cam_yaw_r = 0.0f;
	float cam_roll_r = 0.0f;
	float roll_cap_r = 0.1f;
	float roll_rate_rps = 0.5; //radians per second
	Vector3 pos_m;
	Vector3 velocity_mps;
	Vector3 forward_dir;
	Vector3 strafe_dir;
	Vector3 collision_vec_m;
	PLAYERSTATE state = AIR;

	bool editor_noclip = true;
	float editor_acel_mult = 4.0f;

	float mouseSensitivityX = 4.f;
	float mouseSensitivityY = 4.f;

	Vector3 dash_dir;
	float boost_dist_m = 10.0f;     //How far boost moves your position
	timer_t boost_timer;
	timer_t boost_cooldown;
	Vector3 boost_start_pos_m, boost_end_pos_m;

	timer_t gun_cooldown;
	Vector3 bullet_spawn_offset_m = { 0.3f, -0.5f, 0.0f };
	float bullet_target_distance_m = 30.0f;
	const int starting_ammo_count = 20;
	int ammo_count = starting_ammo_count;
	int gun_damage = 2;
	float ammo_gained_per_melee_damage = 0.1f;

	float bullet_speed_mps = 100.0f;
	float bullet_radius_m = 0.2f;
	float bullet_sheild_radius_m = 2.0f;

	const float melee_hurtbox_offset_m = 2.0f;
	OBB_t melee_hurtbox;
	float melee_hurtbox_width = 4.0f;
	float melee_hurtbox_depth = 4.0f;
	float melee_hurtbox_height = 1.5f;
	int melee_damage = 10;
	timer_t melee_active_timer;
	timer_t melee_cooldown_timer;

	bool look_trigger_active = false;

	timer_t jump_timer;
	float jump_init_impulse_mps = 10.0f;

	bool can_grapple = false;
	Vector3 grapple_point = { 0,0,0 };
	bool in_grapple = false;
	timer_t grapple_cooldown_timer;
	float grapple_max = 25.0f;
	float grapple_acel_mps2 = 9.1f;
	float max_grapple_dist_scaling = 20.0f;

	float wall_jump_off = 1.5f;
	float wall_jump_up = 1.5f;

	float base_acel_mps2 = 40.0f;
	float terminal_vel_y_mps = -700.0f;

	float air_friction_mux = 2.0f;
	float ground_friction_mux = 3.0f;
	float wall_friction_y = 0.95f;
	float wall_friction_tangent = 0.96f;

	float gravity_mps2 = 19.8f; 

	//Visuals
	Model melee_model;
	Model arm_model;
	ModelAnimation* anims;
	unsigned int anim_count = 0;
	timer_t melee_anim_timer;

	float fixup_x, fixup_y = -1.85f, fixup_z = -0.44f;
	Vector3 fixup_pos;

	void init() {
		fixup_pos.x = 0.06f;
		fixup_pos.y = 0.15f;
		fixup_pos.z = 0.05f;

		invuln_timer.freq_hz = 1.0f/0.2f;

		jump_timer.freq_hz = 1.0f/0.5f;
		jump_timer.accum_s = -1.0f;

		boost_cooldown.freq_hz = 1.0f/2.0f;
		boost_cooldown.accum_s = -1.0f;

		boost_timer.freq_hz = 1.0f/0.1f;
		boost_timer.accum_s = -1.0f;

		gun_cooldown.freq_hz = 7.0f;

		melee_hurtbox.e[0] = melee_hurtbox_depth/2;
		melee_hurtbox.e[1] = melee_hurtbox_height/2;
		melee_hurtbox.e[2] = melee_hurtbox_width/2;

		melee_cooldown_timer.freq_hz = 1.0f/0.5;
		melee_cooldown_timer.accum_s = -1.0f;

		melee_active_timer.freq_hz = 1.0f/0.1;
		melee_active_timer.accum_s = -1.0f;

		grapple_cooldown_timer.freq_hz = 1.0f/2.0f;

		melee_anim_timer.freq_hz = 1.0f/0.4f;
		melee_anim_timer.accum_s = -1.0f;

		player_camera.position = Vector3{ 10,1,1 };
		player_camera.target = Vector3{ 1, 1, 0 };
		player_camera.up = UNIT_VECS[DIR_Y];
		player_camera.fovy = 95.0f;
		player_camera.projection = CAMERA_PERSPECTIVE;

		health = max_health;
		ammo_count = starting_ammo_count;
		if(!melee_model.meshes) {
			melee_model = LoadModel(asset_dir "bill.iqm");
			melee_model.materials->shader = ground_shader;
		}

		if(!arm_model.meshes) {
			arm_model = LoadModel(asset_dir "arm.iqm");
			anims = LoadModelAnimations(asset_dir "arm_anim.iqm", &anim_count);
			arm_model.materials->shader = ground_shader;
			for(int i = 0; i < anim_count; i++) {
				assert(IsModelAnimationValid(arm_model, anims[i]));
			}
		}
	}

	void update() {
		if(editor_mode) {
			//@bug debug_drawer.add_cross(pos_m, 0.5f, RED, 0.0f, true);
			ADD_CROSS(pos_m, 0.5f, RED, 0.0f, true);
			ADD_CROSS(pos_m + Vector3{ 0,0,10 }, 0.5f, RED, 0.0f, true);
		}

		debug_drawer.add_string(Vector3{ 0.1f, 0.9f, 0 }, 30, false, RED, 0.0f, "AMMO: %i", ammo_count);
		debug_drawer.add_string(Vector3{ 0.1f, 0.8f, 0 }, 30, false, GREEN, 0.0f, "Health: %i", health);
		debug_drawer.add_string(Vector3{ 0.1f, 0.7f, 0 }, 30, false, RED, 0.0f, "Speed = %f", Vector3Length(velocity_mps));
		debug_drawer.add_string(Vector3{ 0.1f, 0.2f, 0 }, 30, false, RED, 0.0f, "pos: X %.2f Y %.2f Z %.2f ", pos_m.x, pos_m.y, pos_m.z);
		//debug_drawer.add_string(Vector3{ 0.3f, 0.3f, 0 }, 60, false, RED, 0.0f, "collision state = %i", state);

		bool can_dash = boost_cooldown.tick(dt_s) && health > 0;
		bool can_shoot = gun_cooldown.tick(dt_s) && ammo_count > 0 && health > 0;
		bool can_swing = melee_cooldown_timer.tick(dt_s) && health > 0;
		can_grapple = true;//grapple_cooldown_timer.tick(dt_s) && health > 0;
		invuln_timer.tick(dt_s);

		melee_anim_timer.tick(dt_s);
		melee_active_timer.tick(dt_s);

		look_trigger_active = false;
		//Find forward and strafe vectors
		//remove and normalize forward vec so you can't gain speed by looking up and also don't move slow
		look_dir = Vector3Normalize(player_camera.target - player_camera.position);
		assert(Vector3Length(look_dir) > 0);
		forward_dir = look_dir;
		forward_dir.y = 0;
		dash_dir = Vector3Normalize(forward_dir);
		forward_dir = dash_dir;
		strafe_dir = Vector3Normalize(Vector3CrossProduct(look_dir, UNIT_VECS[DIR_Y]));

		// check INPUT_FLAGS 
		Vector3 dir = { 0,0,0 };
		if(inputs.input_state & INPUT_FLAGS::W) {
			dir += forward_dir;
		}

		if(inputs.input_state & INPUT_FLAGS::S) {
			dir -= forward_dir;
		}

		if(inputs.input_state & INPUT_FLAGS::A) {
			dir -= strafe_dir;
			if(cam_roll_r > -roll_cap_r && camera_roll)
				cam_roll_r -= roll_rate_rps*dt_s;
		}

		if(inputs.input_state & INPUT_FLAGS::D) {
			dir = Vector3Add(dir, strafe_dir);
			if(cam_roll_r < roll_cap_r && camera_roll)
				cam_roll_r += roll_rate_rps*dt_s;
		}

		if(Vector3Length(dir) > 0.0f) {
			dir = Vector3Normalize(dir);
		}

		dash_dir = dir;
		Vector3 acel_dir_mps2 = dir*base_acel_mps2;
		if(health > 0) {
			if(editor_mode) {
				velocity_mps += acel_dir_mps2*editor_acel_mult*dt_s;
			}
			else {
				velocity_mps += acel_dir_mps2*dt_s;
			}
		}

		if(inputs.input_state & INPUT_FLAGS::LEFT_SHIFT) {
			if(editor_mode) {
				velocity_mps.y -= base_acel_mps2*editor_acel_mult*dt_s;
			}
			else {
				if(can_dash) {
					boost_cooldown.reset();
					boost_timer.reset();
					boost_start_pos_m = pos_m;
					if(Vector3Length(dash_dir) < 0.1f) {
						dash_dir = Vector3Normalize(Vector3{ look_dir.x, 0, look_dir.z });
					}
					boost_end_pos_m = pos_m + dash_dir*boost_dist_m;
				}
			}
		}

		if(inputs.input_down & INPUT_FLAGS::MMB && can_grapple) {
			Ray r = { player_camera.position, look_dir };
			float tmin;
			Vector3 q;
			if(collision_manager.cast_ray(r, OBJECT_TYPE::TYPE_LEVEL_GEO, &tmin, &q)) {
				if(tmin < grapple_max) {
					grapple_point = q;
					in_grapple = true;
					audio_manager.add_sound_fx_positional(SOUNDS::grapple, 1.0f, q, true);
				}
			}
		}

		if(inputs.input_state & INPUT_FLAGS::MMB) {
			if(in_grapple) {
				Vector3 grapple_vec = grapple_point - player_camera.position;
				velocity_mps += Vector3Normalize(grapple_vec)*(min(Vector3Length(grapple_vec), max_grapple_dist_scaling)*grapple_acel_mps2*dt_s);
				grapple_cooldown_timer.reset();
			}
		}

		if(inputs.input_up & INPUT_FLAGS::MMB) {
			in_grapple = false;
		}

		if(inputs.input_down & INPUT_FLAGS::E && !editor_mode) {
			look_trigger_active = true;
			//audio_manager.add_sound_fx(SOUNDS::laser, 0.1f, Vector3Zero());
			//audio_manager.add_sound_fx_set_duration(SOUNDS::laser, 0.1f, pos_m, 0.1f);
			//particle_system.activate_particles(10, pos_m + look_dir*1000, UNIT_VECS[DIR_X]*100, WHITE, 120, true, 2);
		}

		//apply dash
		if(boost_timer.accum_s >= 0.0f) {
			pos_m = Vector3Lerp(boost_start_pos_m, boost_end_pos_m, 1 - boost_timer.accum_s/(1/boost_timer.freq_hz)) ;
			if(velocity_mps.y < 0) {
				velocity_mps.y = 0.0f;
			}
			boost_timer.tick(dt_s);
		}

		if(!editor_mode) {
			if(inputs.input_state & INPUT_FLAGS::LMB && can_swing) {
				melee_anim_timer.reset();
				melee_cooldown_timer.reset();
				melee_active_timer.reset();
				audio_manager.add_sound_fx(SOUNDS::swoosh, 1.0f, true);
			}
		}

		if(!editor_mode) {
			if(melee_active_timer.accum_s > 0.0f) {
				//update hurtbox
				melee_hurtbox.c = pos_m + UNIT_VECS[DIR_Y]*eye_height_m + look_dir*melee_hurtbox_offset_m;
				melee_hurtbox.u[2] = look_dir;

				Matrix r = MatrixRotate(look_dir, PI/2);
				melee_hurtbox.u[0] = Vector3Normalize(Vector3Transform(Vector3CrossProduct(strafe_dir, look_dir), r));
				melee_hurtbox.u[1] = Vector3CrossProduct(melee_hurtbox.u[0], melee_hurtbox.u[2]);
			}
		}

		if(inputs.input_state & INPUT_FLAGS::RMB && !editor_mode) {
			if(can_shoot) {
				//This is silly, we should just be using a dif coord system for this
				Vector3 bullet_spawn_pos = strafe_dir*bullet_spawn_offset_m.x + player_camera.up*bullet_spawn_offset_m.y + look_dir*bullet_spawn_offset_m.z;
				bullet_spawn_pos += player_camera.position;
				Vector3 target_pos = player_camera.position + look_dir*bullet_target_distance_m;

				for(int i = 0; i < 5; i++) {
					Vector3 bullet_dir = Vector3Normalize(target_pos - bullet_spawn_pos + Vector3{ rand_ndc(), rand_ndc(), rand_ndc() });
					bullet_manager.add_bullet(bullet_spawn_pos, bullet_dir, bullet_speed_mps, bullet_radius_m, bullet_sheild_radius_m, gun_damage, true);
				}
				gun_cooldown.reset();
				ammo_count--;
				audio_manager.add_sound_fx(SOUNDS::newlazer, 0.5f, true);
			}
		}

		switch(state)
		{
		case AIR:
		{
			jump_timer.tick(dt_s);

			if(inputs.input_state & INPUT_FLAGS::SPACE) {
				if(editor_mode) {
					velocity_mps.y += base_acel_mps2*editor_acel_mult*dt_s;
				}

				if(jump_timer.accum_s > 0.0f) {
					//velocity_mps.y += jump_hold_acel_mps2*dt_s;
				}
			}
			else {

			}

			if(!editor_mode) {
				Vector3 acel_mps2 = velocity_mps*-air_friction_mux;

				velocity_mps.x = velocity_mps.x + (acel_mps2.x*dt_s);
				velocity_mps.z = velocity_mps.z + (acel_mps2.z*dt_s);
			}
			else {
				Vector3 acel_mps2 = velocity_mps*-air_friction_mux*2;
				velocity_mps += acel_mps2*dt_s;
			}
			break;
		}
		case GROUND:
		{
			if(inputs.input_down & INPUT_FLAGS::SPACE && health > 0) {
				velocity_mps.y += jump_init_impulse_mps;
				jump_timer.reset();
				state = AIR;
				audio_manager.add_sound_fx(SOUNDS::dirthit, 0.5f, true);
			}
#if 0
			Vector3 velocity_dir = Vector3Normalize(velocity_mps);
			Vector3 acel_mps2 = Vector3Scale(velocity_dir, -pow(Vector3Length(velocity_mps), 0.5f));
#endif
			Vector3 acel_mps2 = velocity_mps*-ground_friction_mux;
			velocity_mps += acel_mps2*dt_s;

			break;
		}
#if 0
		case WALL:
		{
			Vector3 wallFriction = { 1.0, 1.0, 1.0 };
			Vector3 wallJump;
			if(collisionDir == LEFT)
			{
				//velocity.x -= 0.03; //stickyness
				wallFriction = { 1.0, wallFrictionY, wallFrictionTangent };
				wallJump = { wallJumpOff, wallJumpUp, 0.0 };
			}
			else if(collisionDir == RIGHT)
			{
				//velocity.x += 0.03; //stickyness
				wallFriction = { 1.0, wallFrictionY, wallFrictionTangent };
				wallJump = { -wallJumpOff, wallJumpUp, 0.0 };
			}
			else if(collisionDir == FRONT)
			{
				wallFriction = { wallFrictionTangent, wallFrictionY, 1.0 };
				wallJump = { 0.0, wallJumpUp, -wallJumpOff };
			}
			else if(collisionDir == BACK)
			{
				wallFriction = { wallFrictionTangent, wallFrictionY, 1.0 };
				wallJump = { 0.0, wallJumpUp, wallJumpOff };
			}

			//	velocity_mps = Vector3Multiply(velocity_mps, wallFriction);
		}
#endif
		}
		//debug_drawer.add_string(Vector3{ .5,.5,0 }, 60, false, RED, 0.0f, "%f", cam_roll_r);
		if(!(inputs.input_state & INPUT_FLAGS::A) && !(inputs.input_state & INPUT_FLAGS::D) && abs(cam_roll_r) > 0.005f) {
			if(cam_roll_r > 0)
				cam_roll_r -= roll_rate_rps*dt_s;
			else
				cam_roll_r += roll_rate_rps*dt_s;
		}
		// Gravity!
		if(velocity_mps.y > terminal_vel_y_mps) {
			if(!editor_mode) {
				velocity_mps.y -= gravity_mps2*dt_s;
			}
		}

		// make sure player can't look too far down
		if(cam_pitch_r > 1.55f)
			cam_pitch_r = 1.55f;

		if(cam_pitch_r < -1.55f)
			cam_pitch_r = -1.55f;

		// make sure players yaw stays within 360 degrees
		cam_yaw_r = fmod(cam_yaw_r +PI*2, PI*2);

		pos_m += velocity_mps*dt_s;

		// Set the camera position to the players position
		player_camera.position = pos_m;
		player_camera.position.y += eye_height_m;

		player_camera.target = UNIT_VECS[DIR_Z];

		Matrix rot = MatrixRotateXYZ_old(Vector3{ cam_pitch_r, -cam_yaw_r, 0 });

		if(health <= 0) { cam_roll_r = PI/2; player_camera.position.y -= eye_height_m/4; }
		player_camera.up = Vector3Transform(UNIT_VECS[DIR_Y], MatrixRotate(look_dir, cam_roll_r));

		player_camera.target = Vector3Transform(player_camera.target, rot);

		player_camera.target = player_camera.target + player_camera.position;

		// Set players collisionbox to player coords
		bounding_box.min = pos_m + Vector3({ -width_m/2, 0, -width_m/2 });
		bounding_box.max = pos_m + Vector3({ width_m/2, height_m, width_m/2 });

		bottom_bounding_box.min = pos_m - Vector3{ width_m/2, downward_collision_margin_m, width_m/2 };
		bottom_bounding_box.max = pos_m + Vector3{ width_m/2, max_stepable_stair_height_m, width_m/2 };
		state = AIR;
	}

	void take_damage(int damage) {
		if(invuln_timer.accum_s < 0.0f && health > 0) {
			health -= damage;
			invuln_timer.reset();
			audio_manager.add_sound_fx(SOUNDS::unf, 1.0f, true);
			debug_drawer.add_string(Vector3{ 0.5f, 0.5f, 0.0f }, 100, false, RED, 1.0f, "OUCH!");
		}
	}

	void draw() {
		//melee_model.transform = interpolate_transforms(melee_start_transform, melee_end_transform, Clamp(melee_anim_timer.accum_s/(1/melee_anim_timer.freq), 0, 1));
		float norm =  1.0f - (melee_anim_timer.accum_s/(1.0f/melee_anim_timer.freq_hz));
		norm = Clamp(norm, 0.0f, 1.0f);
		int frame = (int)(norm*(float)anims[0].frameCount);
		if(norm <= 0.0f) { frame = 1; }
        if (frame >= anims[0].frameCount) frame = frame%anims[0].frameCount;
		UpdateModelAnimation(arm_model, anims[0], frame);

		Vector3 melee_pos = player_camera.position - strafe_dir*0.5f - Vector3CrossProduct(strafe_dir, look_dir)*0.2f + look_dir*0.1f;

		Matrix matScale = MatrixScale(1.00f, 1.00f, 1.00f);
		Matrix matRotation = MatrixRotate(UNIT_VECS[DIR_Y], cam_yaw_r);
		matRotation = MatrixMultiply(MatrixRotate(UNIT_VECS[DIR_X], -cam_pitch_r), matRotation);

		matRotation = MatrixMultiply(MatrixRotateXYZ_old(Vector3{ 90.0f*DEG2RAD, 0.f*DEG2RAD, 0.0f*DEG2RAD }), matRotation);

		Matrix matTranslation = MatrixTranslate(melee_pos.x, melee_pos.y, melee_pos.z);
		Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

		DrawMesh(arm_model.meshes[0], arm_model.materials[arm_model.meshMaterial[0]], matTransform);


		Vector3 pos = anims[0].framePoses[frame][3].translation;
		Quaternion rot = anims[0].framePoses[frame][3].rotation;

		pos = Vector3Transform(pos, matTransform);
		//debug_drawer.add_cross(__COUNTER__, pos, 0.3f);

		Matrix melee_transform = MatrixTranslate(fixup_pos.x, fixup_pos.y, fixup_pos.z);
		melee_transform = MatrixMultiply(melee_transform, MatrixMultiply(QuaternionToMatrix(rot), matRotation));
		melee_transform = MatrixMultiply(MatrixRotateXYZ_old(Vector3{fixup_x, fixup_y, fixup_z }), melee_transform);
		melee_transform = MatrixMultiply(melee_transform, MatrixTranslate(pos.x, pos.y, pos.z));

		DrawMesh(melee_model.meshes[0], melee_model.materials[melee_model.meshMaterial[0]], melee_transform);
		DrawMesh(melee_model.meshes[1], melee_model.materials[melee_model.meshMaterial[0]], melee_transform);

		if(in_grapple) {
			Vector3 grapple_to_me = grapple_point - pos_m;
			float len = Vector3Length(grapple_to_me);
			float width = Remap(len, 0.0f, grapple_max, 0.1f, 0.01f);
	 		DrawCylinder(pos_m, grapple_to_me, width, width, len, 10, RED);
			//DrawLine3D(pos_m, grapple_point, RED);
		}
		if(melee_anim_timer.accum_s > 0) {
			DrawOBBWires(melee_hurtbox.c, melee_hurtbox.u[0], melee_hurtbox.u[1], melee_hurtbox.u[2], Vector3{ melee_hurtbox.e[0],melee_hurtbox.e[1],melee_hurtbox.e[2] }, PURPLE);
		}
	}

	void draw_hud() {
		Vector2 center = norm_to_screen_coords(Vector2One()*0.5f, Vector2{ (float)screen_width, (float)screen_height });
		Ray r = { player_camera.position, look_dir };
		float tmin;
		Vector3 q;

		float rad = 10.0f;
		Color c = RED;
		if(collision_manager.cast_ray(r, OBJECT_TYPE::TYPE_LEVEL_GEO, &tmin, &q)){
			rad = Remap(tmin, 0, grapple_max, 1.0f, 5.0f);
			rad = Clamp(rad, 1.0f, 10.0f);
			if(tmin < grapple_max && can_grapple) { c = GREEN; }
		}

		DrawCircle((int)center.x, (int)center.y, rad, c);

		Vector2 left_bottom = norm_to_screen_coords(Vector2{0.1f, 0.8f}, Vector2{ (float)screen_width, (float)screen_height });

		const float ui_rad = norm_to_screen_scale(0.04f, Vector2{ (float)screen_width, (float)screen_height });
		float health_rad = health < max_health/3 ? ui_rad + sin(game_time_s*6)*ui_rad/8 : ui_rad;
		float angle = Remap(health, 0.0f, (float)max_health, 0.0f, 360.0f);
		DrawCircleSector(left_bottom, health_rad, 0, (int)angle, 360, RED);

		Vector2 right_bottom = norm_to_screen_coords(Vector2{ 0.9f, 0.8f, }, Vector2{ (float)screen_width, (float)screen_height });

		angle = Remap((float)ammo_count, 0.0f, starting_ammo_count*2.0f, 0.0f, 360.0f);
		DrawCircleSector(right_bottom, ui_rad, 0, (int)angle, 360, GREEN);
	}
};


#if ATGM_TESTING
struct ATGM_t {
	Vector3 start_pos = Vector3Zero();
	Vector3 shoot_dir = UNIT_VECS[DIR_Z];
	float speed_mps = 1.0f;

	int eval_num = 100;
	float eval_ps = 1.0f;

	float jitter = 1.0f;
	float jitter_dist_scale = 0.0f;

	float side_to_side_radius_m = 1.0f;
	float side_to_side_radius_dist_scale = 0.0f;

	float side_to_side_period_m = 1.0f;
	float side_to_side_period_scale = 1.0f;

	float up_down_radius_m = 1.0f;
	float up_down_radius_dist_scale = 0.0f;

	float up_down_period_m = 1.0f;
	float up_down_period_scale = 1.0f;

	void draw() {

		Vector3 point = start_pos;
		float dist_m = 0.0f;

		Vector3 side_dir = UNIT_VECS[DIR_X];
		Vector3 up_dir = UNIT_VECS[DIR_Y];

		for (int i = 0; i < eval_num; i++) {
			dist_m = (speed_mps/eval_ps)*i;

			Vector3 side_to_side = sin(dist_m*side_to_side_period_m + side_to_side_period_scale*dist_m)*side_dir*side_to_side_radius_m * (side_to_side_radius_dist_scale*dist_m);
			Vector3 up_down = cos(dist_m*up_down_period_m + up_down_period_scale*dist_m) * up_dir * up_down_radius_m + up_dir * (up_down_radius_dist_scale * dist_m);

			point = start_pos + shoot_dir*dist_m + side_to_side + up_down;

			DrawSphereEx(point, 0.1f, 3, 3, RED);
		}
	}

};
#endif
