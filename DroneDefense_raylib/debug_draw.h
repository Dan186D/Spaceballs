#pragma once

#include "pch.h"
#include "common.h"


//@BUG fix this and make it actually work with all the debug types, you need to make sure if you are doing loops while debug drawing that you call the original function and
// multiply __COUNTER__ by some largish number + idx so that it does not collide with itself or other __COUNTER__s

using namespace std;

enum DEBUG_DRAWABLE {DEBUG_VEC, DEBUG_CROSS, DEBUG_SPHERE, DEBUG_CYLINDER, DEBUG_AXES, DEBUG_RAY, DEBUG_LINE, DEBUG_AABB, DEBUG_CIRCLE, DEBUG_OBB, DEBUG_STRING};

struct debug_draw_info_t {
	DEBUG_DRAWABLE type;
	Color col;
	float duration_s;
	float max_duration_s;
	bool depth_test;
};

struct debug_vec_t {
	debug_draw_info_t d;
	Vector3 pos_m;
	Vector3 vec_m;
	float end_cap_radius_m;
	float cone_length_alpha;

	void draw() {
		DrawCylinder(pos_m, vec_m, end_cap_radius_m/2, end_cap_radius_m/2, Vector3Length(vec_m)*(1 - cone_length_alpha), 6, d.col);
		DrawCylinder(pos_m + vec_m*(1 - cone_length_alpha), vec_m, 0, end_cap_radius_m, Vector3Length(vec_m)*cone_length_alpha, 6, d.col);
	}
};

struct debug_sphere_t {
	debug_draw_info_t d;
	Vector3 pos_m;
	float radius_m;

	void draw() {
		DrawSphereEx(pos_m, radius_m, 6, 6, d.col);
	}
};

struct debug_cylinder {
	debug_draw_info_t d;
	Vector3 pos_m;
	Vector3 angle;
	float top_radius_m;
	float bottom_radius_m;
	float length_m;

	void draw() {
		DrawCylinder(pos_m, angle, top_radius_m, bottom_radius_m, length_m, 6, d.col);
	}
};

struct debug_axes_t {
	debug_draw_info_t d;
	Vector3 pos_m;
	float size_m;

	void draw() {
		DrawLine3D(pos_m, pos_m + UNIT_VECS[DIR_X]*size_m, RED);
		DrawLine3D(pos_m, pos_m + UNIT_VECS[DIR_Y]*size_m, GREEN);
		DrawLine3D(pos_m, pos_m + UNIT_VECS[DIR_Z]*size_m, BLUE);
	}
};

struct debug_ray_t {
	debug_draw_info_t d;
	Ray ray;

	void draw() {
		DrawRay(ray, d.col);
	}
};

struct debug_line_t {
	debug_draw_info_t d;
	Vector3 p0, p1;

	void draw() {
		DrawLine3D(p0, p1, d.col);
	}
};

struct debug_cross_t {
	debug_draw_info_t d;
	Vector3 pos_m;
	float size_m;

	void draw() {
		DrawLine3D(pos_m + UNIT_VECS[DIR_X]*(size_m/2.0f), pos_m - UNIT_VECS[DIR_X]*(size_m/2.0f), d.col);
		DrawLine3D(pos_m + UNIT_VECS[DIR_Y]*(size_m/2.0f), pos_m - UNIT_VECS[DIR_Y]*(size_m/2.0f), d.col);
		DrawLine3D(pos_m + UNIT_VECS[DIR_Z]*(size_m/2.0f), pos_m - UNIT_VECS[DIR_Z]*(size_m/2.0f), d.col);
	}
};

#define DEBUG_STRING_SIZE 100
struct debug_string_t {
	debug_draw_info_t d;
	Vector3 pos;
	float size;
	bool three_D;
	char string[DEBUG_STRING_SIZE];

	void draw() {
		Vector2 screen_pos_px;
		float text_scale = 1.0f;
		if(three_D) {
			//if the text is behind us don't back project it
			if(Vector3DotProduct(player_camera.target - player_camera.position, pos - player_camera.position) < 0.5f) {
				return;
			}
			//project 3d point to screen space
			text_scale = Clamp(1.0f/Vector3Length(player_camera.position - pos)*10, 0.01f, 1.0f);
			screen_pos_px = GetWorldToScreen(pos, player_camera);
		}else{
			screen_pos_px = norm_to_screen_coords(Vector2{ pos.x, pos.y }, Vector2{ (float)screen_width, (float)screen_height });
		}

		if(d.max_duration_s == 0.0f) {
			DrawTextEx(GetFontDefault(), string, screen_pos_px, size*text_scale, 1.0f, d.col);
		} else {
			DrawTextEx(GetFontDefault(), string, screen_pos_px, size*text_scale, 1.0f, lerp(Color{ d.col.r, d.col.g, d.col.b, 0 }, d.col, d.duration_s/d.max_duration_s));
		}
	}
};

struct debug_AABB_t {
	debug_draw_info_t d;
	Vector3 min, max;

	void draw() {
		Vector3 dimentions = max - min;
		Vector3 center = min + dimentions*0.5f;
		DrawCubeWires(center, dimentions.x, dimentions.y, dimentions.z, d.col);
	}
};

struct debug_OBB_t {
	debug_draw_info_t d;
	OBB_t obb;

	void draw() {
		DrawOBBWires(obb.c, obb.u[0], obb.u[1], obb.u[2], Vector3{obb.e[0], obb.e[1], obb.e[2]}, d.col);
	}
};

struct debug_circle_t {
	debug_draw_info_t d;
	Vector3 pos_m;
	float radius_m;
	Vector3 rot_axis;

	void draw() {
		Vector3 axis = Vector3CrossProduct(rot_axis, Vector3{ 0, 0, 1 });
		float angle = RAD2DEG*acosf(Vector3DotProduct(rot_axis, Vector3{ 0, 0, 1 }));
		DrawCircle3D(pos_m, radius_m, axis, angle, d.col);
	}
};

union debug_drawable_t {
	debug_draw_info_t d;
	debug_sphere_t sph;
	debug_cylinder cyl;
	debug_axes_t axs;
	debug_cross_t crs;
	debug_ray_t ray;
	debug_line_t lin;
	debug_vec_t vec;
	debug_AABB_t BB;
	debug_OBB_t OBB;
	debug_circle_t cir;
	debug_string_t str;
};

struct debug_drawer_t {
	unordered_map<int, debug_drawable_t> drawables;

	void add_or_overwrite(int id, debug_drawable_t drawable) {
		auto found_element = drawables.find(id);
		if (found_element != drawables.end()) {
			assert(found_element->second.d.type == drawable.d.type);
			found_element->second = drawable;
		}
		else {
			drawables.emplace(id, drawable);
		}
	}

	void draw_debug() {
		assert(drawables.size() < 300);
#if _DEBUG
		for(int i = 0; i < drawables.size(); i++) {
			debug_drawable_t* drawable = &drawables[i];
			DEBUG_DRAWABLE type = drawable->d.type;

			if(drawable->d.depth_test == false) {
				//@@PERF: THIS IS STUPID!! FIX THIS AND DO DEPTH TESTERS IN SEPRATE BATCHES!!
				rlDrawRenderBatchActive();
				rlDisableDepthTest();
			}

			switch(type)
			{
			case DEBUG_VEC: {
				drawable->vec.draw();
				break;
			}
			case DEBUG_CROSS: {
				drawable->crs.draw();
				break;
			}
			case DEBUG_SPHERE: {
				drawable->sph.draw();
				break;
			}
			case DEBUG_CYLINDER: {
				drawable->cyl.draw();
				break;
			}
			case DEBUG_AXES:
				drawable->axs.draw();
				break;
			case DEBUG_RAY:
				drawable->ray.draw();
				break;
			case DEBUG_LINE:
				drawable->lin.draw();
				break;
			case DEBUG_AABB:
				drawable->BB.draw();
				break;
			case DEBUG_CIRCLE:
				drawable->cir.draw();
				break;
			case DEBUG_OBB:
				drawable->OBB.draw();
				break;
			case DEBUG_STRING:
				//Since this function is called in "3D mode"
				//we cannot draw text now
				break;
			default:
				break;
			}

			if(drawable->d.depth_test == false) {
				rlDrawRenderBatchActive();
				rlEnableDepthTest();
			}
		}
#endif
	}

	void draw_debug_text() {
#if _DEBUG
		for(int i = 0; i < drawables.size(); i++) {
			debug_drawable_t* drawable = &drawables[i];
			DEBUG_DRAWABLE type = drawable->d.type;

			switch(type)
			{
			case DEBUG_STRING:
				drawable->str.draw();
				break;
			default:
				break;
			}
		}
#endif
	}

	void update_drawables(float delta_time_s) {
		for(auto& it = drawables.begin(); it != drawables.end();) {
			it->second.d.duration_s -= delta_time_s;
			if(it->second.d.duration_s < 0.0f) {
				it = drawables.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void add_cross(int id, const Vector3& pos_m, float size_m, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
		debug_drawable_t drawable;
		drawable.crs.d.type = DEBUG_CROSS;
		drawable.crs.d.col = c;
		drawable.crs.d.duration_s = duration_s;
		drawable.crs.d.max_duration_s = duration_s;
		drawable.crs.d.depth_test = depth_test;
		drawable.crs.pos_m = pos_m;
		drawable.crs.size_m = size_m;

		add_or_overwrite(id, drawable);
		//printf("count = %i\n", ID);
#endif
	}

#define ADD_CROSS(pos_m, size_m, ...) \
	debug_drawer.add_cross(__COUNTER__, pos_m, size_m, __VA_ARGS__);

	void add_axes(const Vector3& pos_m, float size_m, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.axs.d.type = DEBUG_AXES;
			drawable.axs.d.duration_s = duration_s;
			drawable.axs.d.max_duration_s = duration_s;
			drawable.axs.d.depth_test = depth_test;
			drawable.axs.pos_m = pos_m;
			drawable.axs.size_m = size_m;

//			drawables.push_back(drawable);
#endif
	}

	void add_ray(const Ray& r, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.ray.d.type = DEBUG_RAY;
			drawable.ray.d.col = c;
			drawable.ray.d.duration_s = duration_s;
			drawable.ray.d.max_duration_s = duration_s;
			drawable.ray.d.depth_test = depth_test;
			drawable.ray.ray = r;

//			drawables.push_back(drawable);
#endif
	}

	void add_line(const Vector3& start, const Vector3& end, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.lin.d.type = DEBUG_LINE;
			drawable.lin.d.col = c;
			drawable.lin.d.duration_s = duration_s;
			drawable.lin.d.max_duration_s = duration_s;
			drawable.lin.d.depth_test = depth_test;
			drawable.lin.p0 = start;
			drawable.lin.p1 = end;

//			drawables.push_back(drawable);
#endif
	}


	void add_sphere(const Vector3& pos_m, float rad_m, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.sph.d.type = DEBUG_SPHERE;
			drawable.sph.d.col = c;
			drawable.sph.d.duration_s = duration_s;
			drawable.sph.d.max_duration_s = duration_s;
			drawable.sph.d.depth_test = depth_test;
			drawable.sph.pos_m = pos_m;
			drawable.sph.radius_m = rad_m;

//			drawables.push_back(drawable);
#endif
	}

	void add_cylinder(const Vector3& pos_m, const Vector3& angle, float top_radius_m, float bottom_radius_m, float length_m, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.cyl.d.type = DEBUG_CYLINDER;
			drawable.cyl.d.col = c;
			drawable.cyl.d.duration_s = duration_s;
			drawable.cyl.d.max_duration_s = duration_s;
			drawable.cyl.d.depth_test = depth_test;
			drawable.cyl.pos_m = pos_m;
			drawable.cyl.angle = angle;
			drawable.cyl.bottom_radius_m = bottom_radius_m;
			drawable.cyl.top_radius_m = top_radius_m;
			drawable.cyl.length_m = length_m;

		//	drawables.push_back(drawable);
#endif
	}

	void add_AABB(const BoundingBox& b, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG 
			debug_drawable_t drawable;
			drawable.BB.d.type = DEBUG_AABB;
			drawable.BB.d.col = c;
			drawable.BB.d.duration_s = duration_s;
			drawable.BB.d.max_duration_s = duration_s;
			drawable.BB.d.depth_test = depth_test;
			drawable.BB.min = b.min;
			drawable.BB.max = b.max;

			//drawables.push_back(drawable);
#endif
	}

	void add_OBB(const OBB_t& b, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.OBB.d.type = DEBUG_OBB;
			drawable.OBB.d.col = c;
			drawable.OBB.d.duration_s = duration_s;
			drawable.OBB.d.max_duration_s = duration_s;
			drawable.OBB.d.depth_test = depth_test;
			drawable.OBB.obb = b;
			//drawables.push_back(drawable);
#endif
	}

	void add_circle(const Vector3& pos_m, float radius_m, Vector3 rot_axis, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.cir.d.type = DEBUG_CIRCLE;
			drawable.cir.d.col = c;
			drawable.cir.d.duration_s = duration_s;
			drawable.cir.d.max_duration_s = duration_s;
			drawable.cir.d.depth_test = depth_test;
			drawable.cir.pos_m = pos_m;
			drawable.cir.radius_m = radius_m;
			drawable.cir.rot_axis = rot_axis;

			//drawables.push_back(drawable);
#endif
	}

	void add_vector(const Vector3& pos_m, const Vector3& vec_m, float end_rad_m, float cone_length_alpha, Color c = RED, float duration_s = 0.0f, bool depth_test = false) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.vec.d.type = DEBUG_VEC;
			drawable.vec.d.col = c;
			drawable.vec.d.duration_s = duration_s;
			drawable.vec.d.max_duration_s = duration_s;
			drawable.vec.d.depth_test = depth_test;
			drawable.vec.pos_m = pos_m;
			drawable.vec.vec_m = vec_m;
			drawable.vec.end_cap_radius_m = end_rad_m;
			drawable.vec.cone_length_alpha = cone_length_alpha;

			//drawables.push_back(drawable);
#endif
	}

	void add_string(const Vector3& pos_px, float size_px, bool three_D, Color c = RED, float duration_s = 0.0f, const char* format = "%s\n", ...) {
#if _DEBUG
			debug_drawable_t drawable;
			drawable.str.d.type = DEBUG_STRING;
			drawable.str.d.col = c;
			drawable.str.d.duration_s = duration_s;
			drawable.str.d.max_duration_s = duration_s;

			va_list list;
			va_start(list, format);
			vsprintf(drawable.str.string, format, list);
			va_end(list);

			drawable.str.three_D = three_D;
			drawable.str.pos = pos_px;
			drawable.str.size = size_px;

			//drawables.push_back(drawable);
#endif
	}

};
