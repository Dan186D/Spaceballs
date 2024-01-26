#pragma once
#include "pch.h"
#include "io.h"

#include "Entity.h"
#include "common.h"

struct level_t {
	char level_filename[CUTE_FILES_MAX_FILENAME] = "New.txt";
	char io_line[1024] = { 0 };
	char io_error[1024] = { 0 };

	Vector3 start_pos = Vector3{ 0.0f, 0.0f, 0.0f };
	float start_pitch = 0;
	float start_yaw = 0;
	BoundingBox end_box = { 0 };
	light_t lights[NUM_LIGHTS] = { 0 };
	vector<level_geo_t*> level_geo;
	enemy_spawner_t spawners[MAX_SPAWNERS];
	int total_enemies = 0;
	text_trigger_t text_triggers[MAX_TEXT_TRIGGERS];

	float fogLevelMin = -100;
	float fogLevelMax = 200;
	Vector3 clearColour = { 0 };
	Vector3 skyCol1 = Vector3{ 0.101961f, 0.619608f, 0.666667f };
	Vector3 skyCol2 = Vector3{0.666667f, 0.666667f, 0.498039f};
	Vector3 skyCol3 = Vector3{ 0.f, 0.f, 0.164706f };
	Vector3 skyCol4 = Vector3{ 0.666667f, 1.f, 1.f };

	//deletes all world geo, models, and empties the level geo vector
	void empty() {
		for(auto i: level_geo) {
			if(i->model != null) {
				delete i->model;
				for(auto j: level_geo) {
					if(i != j && j->model == i->model) {
						j->model = null;
					}
				}
				i->model = null;
			}
			delete i;
			i = null;
		}
		level_geo.clear();
	}

	//Tries to load model and create level geo, then pushes back to geo array
	level_geo_t* load_model_for_level_geo(const char* model_file_name, level_geo_t* e) {
		char model_path_name[CUTE_FILES_MAX_PATH];
		path_concat(asset_dir, model_file_name, model_path_name, CUTE_FILES_MAX_PATH);

		for(auto i: level_geo) {
			if(!_stricmp(i->model_file_name, model_file_name)) {
				e->model = i->model;
				break;
			}
		}

		if(e->model == null){
			Model* mod = new Model;
			*mod = LoadModel(model_path_name);
			e->model = mod;
		}
		strcpyn(e->model_file_name, model_file_name, CUTE_FILES_MAX_FILENAME);
		level_geo.push_back(e);
		return e;
	}

	void compute_geo_bbs() {
		for(auto i : level_geo) {
			i->compute_bounding_box();
		}

		//@@HACK!! remove later
		//compute enemy totals
		total_enemies = 0;
		for(int i = 0; i < MAX_SPAWNERS; i++) {
			for(int j = (int)ENEMY_TYPE::ENEMY_MIN; j < (int)ENEMY_TYPE::ENEMY_MAX; j++) {
				if(j != (int)ENEMY_TYPE::ENEMY_FLYER) {
					total_enemies += spawners[i].num_enemy[j];
				}
			}
		}
	}

	void load_model(const char* model_file_name, level_geo_t* e) {
		char model_path_name[CUTE_FILES_MAX_PATH];
		path_concat(asset_dir, model_file_name, model_path_name, CUTE_FILES_MAX_PATH);

		//TODO: LEAK: do not leak the model lol
		e->model = null;
		for(auto i: level_geo) {
			if(!_stricmp(i->model_file_name, model_file_name)) {
				e->model = i->model;
				break;
			}
		}

		if(e->model == null){
			Model* mod = new Model;
			*mod = LoadModel(model_path_name);
			e->model = mod;
		}
		strcpyn(e->model_file_name, model_file_name, CUTE_FILES_MAX_FILENAME);
	}

	void load_tex(const char* tex_file_name, level_geo_t* e) {
		char tex_path_name[CUTE_FILES_MAX_PATH];
		path_concat(asset_dir, tex_file_name, tex_path_name, CUTE_FILES_MAX_PATH);
		bool shared = false;

		//TODO: LEAK: do not leak the tex 
#if 1
		for(auto i: level_geo) {
			if(!_stricmp(i->tex_file_name, tex_file_name) && i->model->materials->maps->texture.id != 1) {
				e->model->materials[0].maps = i->model->materials[0].maps;
				shared = true;
				break;
			}
		}
#endif

		if(!shared) {
			Texture2D tex = LoadTexture(tex_path_name);
			SetMaterialTexture(&e->model->materials[0], MATERIAL_MAP_DIFFUSE, tex);
		}
		strcpyn(e->tex_file_name, tex_file_name, CUTE_FILES_MAX_FILENAME);

		//apply our texcoord transforms
		for(int i = 0; i < e->model->meshes->vertexCount; i++) {
			//x
			e->model->meshes->texcoords[i*2 + 0] *= e->tex_scale.x;
			e->model->meshes->texcoords[i*2 + 0] += e->tex_offset.x;
			//y
			e->model->meshes->texcoords[i*2 + 1] *= e->tex_scale.y;
			e->model->meshes->texcoords[i*2 + 1] += e->tex_offset.y;
		}
		//@TODO::fix this
		//rlUpdateMesh(e->model->meshes[0], 1, e->model->meshes->vertexCount);
	}

	void init_spawner(int index, Vector3 in_pos) {
		enemy_spawner_t* s = &spawners[index];
		if(!s->active) {

			int ar[(int)ENEMY_TYPE::ENEMY_MAX] = { 0 };
			s->init(ar, in_pos, { in_pos, in_pos + Vector3One() });
		}
	}

	void reset_spawners() {
		for(int i = 0; i < MAX_SPAWNERS; i++) {
			spawners[i].reset();
		}
	}

	//text trigger stuff
	void draw_text_triggers_2d() {
		for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
			text_trigger_t* t = &text_triggers[i];
			if(t->active && t->displaying_text && t->disappear_timer.accum_s > 0) {
				Vector2 screen = { GetScreenWidth(), GetScreenHeight() };
				DrawTextEx(GetFontDefault(), t->str, ndc_to_screen_coords(Vector2{ 0,0 }, screen), 40, 1.0f, BLACK);
			}
		}
	}

	void draw_text_triggers_3d() {
		for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
			text_trigger_t* t = &text_triggers[i];
			if(t->active && t->displaying_text && t->disappear_timer.accum_s > 0) {
				//@TODO:: FIX THIS
				//DrawText3D(GetFontDefault(), t->str, t->text_pos, t->normal_dir, t->angle, 1.0f, 1.0f, 1.0f, true, RED);
			}
		}
	}
	void update_text_triggers() {
		static timer_t particle_timer = { 0.0f, 5.0f };
		bool activate_particles = particle_timer.tick(dt_s);
		for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
			text_trigger_t* t = &text_triggers[i];
			t->disappear_timer.tick(dt_s);
			if(t->disappear_timer.accum_s < 0) { t->displaying_text = false; };
			if(t->active && activate_particles) {
				particle_system.activate_particles_on_bounding_box(1, t->bb, UNIT_VECS[DIR_Y], GOLD, 1.0f);
				particle_timer.reset();
			}
		}
	}

	void add_text_collidables() {
		//trigger box
		uint16_t trigger_collides_with = COL_PLAYER_TRIGGER;
		primitive_t trigger_prim;
		trigger_prim.prim_type = PRIM_TYPE::PRIM_AABB;
		for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
			text_trigger_t* t = &text_triggers[i];
			if(t->active) {
				trigger_prim.prim.aabb = t->bb;
				collision_manager.add_collidable(t, t->bb, OBJECT_TYPE::TYPE_TEXT_TRIGGER, trigger_prim, COL_TEXT_TRIGGER, trigger_collides_with);
			}
		}
	}

	bool io_level(io_t* io) {
		const uint8_t latest_version = 8;
		uint8_t version = latest_version;
		bool io_success = io_uint8(io, version);

		//load deprecated
		if(version == latest_version - 1) {
			//old version
			uint32_t num_geo = level_geo.size();

			io_success = io_success &&
				io_vec3(io, start_pos) &&
				io_float(io, start_pitch) &&
				io_float(io, start_yaw) &&
				io_bounding_box(io, end_box) &&
				io_vec3(io, clearColour) &&
				io_light(io, lights[0]) &&
				io_light(io, lights[1]) &&
				io_light(io, lights[2]) &&
				io_light(io, lights[3]) &&
				io_light(io, lights[4]) &&
				io_enemy_spawner(io, spawners[0]) &&
				io_enemy_spawner(io, spawners[1]) &&
				io_enemy_spawner(io, spawners[2]) &&
				io_enemy_spawner(io, spawners[3]) &&
				io_enemy_spawner(io, spawners[4]) &&
				true;
				
			for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
				io_success = io_success &&
					io_text_trigger_new(io, text_triggers[i]) &&
					true;
			}

			io_success = io_success &&
				io_uint(io, num_geo) &&
				true;

			for(int i = 0; i < num_geo; i++) {
				if(!io->write) {
					level_geo_t* ent = new level_geo_t;
					level_geo.push_back(ent);
				}
				io_success = io_success &&
					io_level_geo(io, (*level_geo[i])) &&
					true;
				if(!io->write) {
				//@todo: share model data between level geo, right now we are creating a new model for every geo.
					Model* mod = new Model;
					char model_path_name[CUTE_FILES_MAX_PATH];
					path_concat(asset_dir, level_geo[i]->model_file_name, model_path_name, CUTE_FILES_MAX_PATH);
					*mod = LoadModel(model_path_name);
					level_geo[i]->model = mod;

					//load tex
					if(strcmp(level_geo[i]->tex_file_name, "")) {
						load_tex(level_geo[i]->tex_file_name, level_geo[i]);
					}
				}
			}
			//end

		}
		else if(version == latest_version){
			//New version
			uint32_t num_geo = level_geo.size();

			io_success = io_success &&
				io_vec3(io, start_pos) &&
				io_float(io, start_pitch) &&
				io_float(io, start_yaw) &&
				io_bounding_box(io, end_box) &&
				io_vec3(io, clearColour) &&
				io_light(io, lights[0]) &&
				io_light(io, lights[1]) &&
				io_light(io, lights[2]) &&
				io_light(io, lights[3]) &&
				io_light(io, lights[4]) &&
				io_enemy_spawner_new(io, spawners[0]) &&
				io_enemy_spawner_new(io, spawners[1]) &&
				io_enemy_spawner_new(io, spawners[2]) &&
				io_enemy_spawner_new(io, spawners[3]) &&
				io_enemy_spawner_new(io, spawners[4]) &&
				true;
				
			for(int i = 0; i < MAX_TEXT_TRIGGERS; i++) {
				io_success = io_success &&
					io_text_trigger_new(io, text_triggers[i]) &&
					true;
			}

			io_success = io_success &&
				io_uint(io, num_geo) &&
				true;

			for(int i = 0; i < num_geo; i++) {
				if(!io->write) {
					level_geo_t* ent = new level_geo_t;
					level_geo.push_back(ent);
				}
				io_success = io_success &&
					io_level_geo(io, (*level_geo[i])) &&
					true;
				if(!io->write) {
				//@todo: share model data between level geo, right now we are creating a new model for every geo.
					Model* mod = new Model;
					char model_path_name[CUTE_FILES_MAX_PATH];
					path_concat(asset_dir, level_geo[i]->model_file_name, model_path_name, CUTE_FILES_MAX_PATH);
					*mod = LoadModel(model_path_name);
					level_geo[i]->model = mod;

					//load tex
					if(strcmp(level_geo[i]->tex_file_name, "")) {
						load_tex(level_geo[i]->tex_file_name, level_geo[i]);
					}
				}
			}
			//end
		}
		else {
			//version is too old or corrupted!
			assert(false);
		}
		return io_success;
	}

	bool save_level(const char* filename) {
		char level_path_name[CUTE_FILES_MAX_PATH];
		path_concat(level_dir, filename, level_path_name, CUTE_FILES_MAX_PATH);
		FILE* fp = fopen(level_path_name, "w");
		//asshurt_f(fp != null, "failed to open writeable file '%s'.", path) { return false; }

		assert(fp != null);

		io_t io;
		io_init(&io, fp, true, io_line, asize(io_line), io_error, asize(io_error));

		const bool io_success =
			io_level(&io);
			true;

		fclose(fp);
		assert(io_success);

		//asshurt_f(io_success, "failed to save %s: %s", path, io.error) { return false; }

		return io_success;	
	}

	bool load_level(const char* filename) {
		strcpy(level_filename, filename);
		char level_path_name[CUTE_FILES_MAX_PATH];
		path_concat(level_dir, filename, level_path_name, CUTE_FILES_MAX_PATH);
		FILE* fp = fopen(level_path_name, "r");
		//asshurt_f(fp != null, "failed to open writeable file '%s'.", path)	{ return false; }

		assert(fp != null);

		io_t io;
		io_init(&io, fp, false, io_line, asize(io_line), io_error, asize(io_error));

		const bool io_success =
			io_level(&io);
			true;

		printf(io.error);
		fclose(fp);
		assert(io_success);

		//asshurt_f(io_success, "failed to save %s: %s", path, io.error) { return false; }

		return io_success;	
	}
};
