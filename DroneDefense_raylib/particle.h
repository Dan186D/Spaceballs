#pragma once
#include "pch.h"
#include "common.h"

#define MAX_PARTICLES 1000

struct particle_t {
	Vector3 pos_m = Vector3Zero();
	float size_m = 1.0f;
	Vector3 velocity_mps = Vector3Zero();
	Color col = WHITE;
	float max_lifetime_s = -1.0f;
	float lifetime_s = -1.0f;
	bool grav = true;

	void init(Vector3 pos, Vector3 vel, Color c, float l, bool g, float s) {
		pos_m = pos;
		velocity_mps = vel;
		col = c;
		lifetime_s = l;
		max_lifetime_s = l;
		grav = g;
		size_m = s;
	}
};

struct particle_system_t {
	particle_t particles[MAX_PARTICLES];
	int particle_i = 0;
	Texture tex = { 0 };

	void init(const char* texture_filename) {
		tex = LoadTexture(texture_filename);
	}

	void update_particles() {
		for(int i = 0; i < MAX_PARTICLES; i++) {
			particle_t* p = &particles[i];

			if(p->lifetime_s > 0.0f) {
				p->pos_m += p->velocity_mps*dt_s;
				if(p->grav) {
					p->velocity_mps.y -= 9.81f*dt_s;
				}
				p->lifetime_s -= dt_s;
			}
		}
	}

	//Returns number of newly activated particles
	int activate_particles(int num, Vector3 pos, Vector3 init_vel, Color col, float lifetime_s, bool grav, float size_m = 0.5f, float rand_vel_mult = 3.0f) {
		int num_activated = 0;
		for(int i = particle_i; i != (particle_i - 1 + MAX_PARTICLES) % MAX_PARTICLES; i = (i + 1) % MAX_PARTICLES) {
			if(num_activated >= num) {
				break;
			}
			particle_t* p = &particles[i];
			if(p->lifetime_s < 0.0f) {
				p->init(pos, init_vel + Vector3{ rand_ndc(), rand_ndc(), rand_ndc() }*rand_vel_mult, col, lifetime_s, grav, size_m);
				num_activated++;
			}
		}
		particle_i = (particle_i + num_activated) % MAX_PARTICLES;
#if _DEBUG
		if(num_activated < num) {
			printf("WARNING: particle system: only %i particles were activated when %i should have been.\n", num_activated, num);
		}
#endif
		return num_activated;
	}

	//spawns particles randomly on 5 outward faces of an aabb
	// warning: treats each face as having equal probability
	void activate_particles_on_bounding_box(int num, const BoundingBox& bb, Vector3 init_vel, Color col, float lifetime_s) {
		for(int i = 0; i < num; i++) {
			uint32_t index = rand_uint(5);
			Vector3 pos = Vector3Zero();
			switch(index)
			{
			case 0:
			pos = { lerp(bb.min.x, bb.max.x, rand_norm()),bb.max.y, lerp(bb.min.z, bb.max.z, rand_norm()) };
			break;
			case 1:
			pos = { bb.max.x, lerp(bb.min.y, bb.max.y, rand_norm()), lerp(bb.min.z, bb.max.z, rand_norm()) };
			break;
			case 2:
			pos = {bb.min.x, lerp(bb.min.y, bb.max.y, rand_norm()), lerp(bb.min.z, bb.max.z, rand_norm()) };
			break;
			case 3:
			pos = { lerp(bb.min.x, bb.max.x, rand_norm()), lerp(bb.min.y, bb.max.y, rand_norm()), bb.min.z };
			break;
			case 4:
			pos = { lerp(bb.min.x, bb.max.x, rand_norm()), lerp(bb.min.y, bb.max.y, rand_norm()), bb.max.z };
			break;
			}

			//debug_drawer.add_cross(pos, 0.1f, RED, 2000, false);

			Vector3 vel = UNIT_VECS[DIR_Y];

			activate_particles(1, pos, vel, col, lifetime_s, false);
		}
	}

	//creates an outward expanding sphere from a point
	void activate_particles_from_point_radius(int num, Vector3 point, float init_vel, Color col, float lifetime_s) {
		for(int i = 0; i < num; i++) {
			float phi = rand_norm()*2*PI;
			float theta = acos(2*rand_norm() - 1);
			Vector3 dir = Vector3{ cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta) };

			activate_particles(1, point, dir*init_vel, col, lifetime_s, false);
		}
	}

	void draw_particles(const Camera& cam) {
		for(int i = 0; i < MAX_PARTICLES; i++) {
			if(particles[i].lifetime_s > 0.0f) {
				particle_t* p = &particles[i];
				Color end_col = p->col;
				end_col.a = 0;
				Color lerp_col = lerp(p->col, end_col,  1 - p->lifetime_s/p->max_lifetime_s);
				DrawBillboard(cam, tex, p->pos_m, p->size_m, lerp_col);
			}
		}
	}

};



