#pragma once

#include "pch.h"

#define _DEBUG 1

#define asset_dir "../assets/"
#define shader_dir asset_dir "shaders/" 
#define tex_dir asset_dir "tex/" 
#define level_dir asset_dir "levels/" 

#define MAX_FILES_IN_DIR 100 

#define null NULL

#define NUM_LIGHTS 5
#define MAX_SPAWNERS 5

#define MAX_TEXT_TRIGGERS 20 

#define ATGM_TESTING 1

extern const unsigned int GAME_TICKS_PER_SECOND = 300;
extern const float dt_s = 1.0f/(float)GAME_TICKS_PER_SECOND;
extern float game_time_s;

extern int killed_enemies;

extern unsigned int screen_width;
extern unsigned int screen_height;

extern bool editor_mode;
extern bool debug_collision_detection;
extern bool debug_enemy;

struct input_state_t;
extern input_state_t inputs;

struct collision_manager_t;
extern collision_manager_t collision_manager;

extern Camera3D player_camera;

extern Shader ground_shader;

struct enemy_t;
const int starting_enemies = 20;
#define MAX_ENEMIES 300
extern enemy_t enemies[MAX_ENEMIES];
struct enemy_manager_t;
extern struct enemy_manager_t enemy_manager;

#define MAX_BULLETS 60
struct bullet_manager_t;
extern bullet_manager_t bullet_manager;

struct text_trigger_manager_t;
extern text_trigger_manager_t text_trigger_manager;

#define OBJ_ID_ROLLOVER 1028;
struct object_id_manager_t;
extern object_id_manager_t ID_map;

struct debug_drawer_t;
extern debug_drawer_t debug_drawer;

struct particle_system_t;
extern particle_system_t particle_system;

struct audio_manager_t;
extern audio_manager_t audio_manager;

#define EPSILON 1e-10f
#define asize(a) (int)(sizeof(a)/sizeof(*a))
#define inl __forceinline
inl void strcpyn(char* dest, const char* source, const size_t capacity) { strncpy(dest, source, capacity); dest[capacity - 1] = '\0'; }

int list_of_files_in_dir(char* out_list[MAX_FILES_IN_DIR], const char* dir_name, const char* ext) {
	cf_dir_t dir;
	int i = 0;
	cf_dir_open(&dir, dir_name);

	//free any mem pointed to by out_list
	for(int i = 0; i < MAX_FILES_IN_DIR; i++) {
		if(out_list[i] != null) {
			delete out_list[i];
		}
	}

	while(dir.has_next) {
		cf_file_t file;
		cf_read_file(&dir, &file);
		if(cf_match_ext(&file, ext)) {
			out_list[i] = new char[CUTE_FILES_MAX_FILENAME];
			strcpy(out_list[i], file.name);
			i++;
		}
		cf_dir_next(&dir);
	}
	cf_dir_close(&dir);
	return i;
}

//math functions
enum DIRECTION {DIR_X, DIR_Y, DIR_Z, DIR_NONE};
Vector3 UNIT_VECS[] = { {1,0,0}, {0,1,0}, {0,0,1} };

Color vec3_to_color(const Vector3 c) {
	Color out_c;
	out_c.r = char(c.x*255);
	out_c.g = char(c.y*255);
	out_c.b = char(c.z*255);
	out_c.a = 255;
	return out_c;
}

float lerp(const float p0, const float p1, const float t) {
	return (1 - t) * p0 + t * p1;
}

char lerp_char(const char p0, const char p1, const char t) {
	return (1 - t) * p0 + t * p1;
}

float round_up_to(float numToRound, float multiple) 
{
    return round(numToRound / multiple) * multiple;
}

Color lerp(const Color c0, const Color c1, const float t) {
	Color out_col;
	out_col.r = lerp(c0.r, c1.r, t);
	out_col.g = lerp(c0.g, c1.g, t);
	out_col.b = lerp(c0.b, c1.b, t);
	out_col.a = lerp(c0.a, c1.a, t);

	return out_col;
}

float rand_norm() {
	return (float)rand()/(float)RAND_MAX;
}

float rand_ndc() {
	return rand_norm() - 0.5f;
}

uint32_t rand_uint(uint32_t up_to) {
	return rand() % up_to;
}

//Operator overloading for Vector2
Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	v = Vector2Add(lhs, rhs);
	return v;
};

Vector2 operator+=(Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	lhs = Vector2Add(lhs, rhs);
	v = lhs;
	return v;
};

Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	v = Vector2Subtract(lhs, rhs);
	return v;
};

Vector2 operator-=(Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	lhs = Vector2Subtract(lhs, rhs);
	v = lhs;
	return v;
};

Vector2 operator-(const Vector2& rhs) {
	Vector2 v;
	v = Vector2Negate(rhs);
	return v;
};

Vector2 operator*(const Vector2& lhs, const float rhs) {
	Vector2 v;
	v = Vector2Scale(lhs, rhs);
	return v;
};

Vector2 operator*(const Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	v = Vector2Multiply(lhs, rhs);
	return v;
};

Vector2 operator/(const Vector2& lhs, const Vector2& rhs) {
	Vector2 v;
	v = Vector2Divide(lhs, rhs);
	return v;
};

bool operator==(const Vector2& lhs, const Vector2& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

//Operator overloading for Vector3
Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	Vector3 v;
	v = Vector3Add(lhs, rhs);
	return v;
};

Vector3 operator+=(Vector3& lhs, const Vector3& rhs) {
	Vector3 v;
	lhs = Vector3Add(lhs, rhs);
	v = lhs;
	return v;
};

Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
	Vector3 v;
	v = Vector3Subtract(lhs, rhs);
	return v;
};

Vector3 operator-=(Vector3& lhs, const Vector3& rhs) {
	Vector3 v;
	lhs = Vector3Subtract(lhs, rhs);
	v = lhs;
	return v;
};

Vector3 operator-(const Vector3& rhs) {
	Vector3 v;
	v = Vector3Negate(rhs);
	return v;
};

Vector3 operator*(const Vector3& lhs, const float rhs) {
	Vector3 v;
	v = Vector3Scale(lhs, rhs);
	return v;
};

Vector3 operator*(const float lhs, const Vector3& rhs) {
	Vector3 v;
	v = Vector3Scale(rhs, lhs);
	return v;
};

bool operator<(const Vector3& lhs, const Vector3& rhs) {
	return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
}

bool operator>(const Vector3& lhs, const Vector3& rhs) {
	return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z;
}

Vector2 screen_coords_to_norm(const Vector2 scr, const Vector2 screen_dims) {
	Vector2 out = scr/screen_dims;
	return out;
}

Vector2 norm_to_screen_coords(const Vector2 norm, const Vector2 screen_dims) {
	Vector2 out = norm*screen_dims;
	return out;
}

Vector2 screen_coords_to_ndc(const Vector2 scr, const Vector2 screen_dims) {
	Vector2 out = scr - screen_dims*0.5f;
	out = out/screen_dims;
	return out;
}

Vector2 ndc_to_screen_coords(const Vector2 ndc, const Vector2 screen_dims) {
	Vector2 out = ndc*screen_dims;
	out = out + screen_dims*0.5f;
	return out;
}

float norm_to_screen_scale(const float scale, const Vector2 screen_dims) {
	return scale*screen_dims.x;
}

Matrix interpolate_transforms(const Matrix& m1, const Matrix& m2, const float t) {
	Quaternion q1 = QuaternionFromMatrix(m1);
	Quaternion q2 = QuaternionFromMatrix(m2);
	Quaternion q_final = QuaternionSlerp(q1, q2, t);

	Matrix m_final = QuaternionToMatrix(q_final);

	Vector3 t1 = Vector3{ m1.m12, m1.m13, m1.m14 };
	Vector3 t2 = Vector3{ m2.m12, m2.m13, m2.m14 };

	Vector3 t_final = Vector3Lerp(t1, t2, t);

	m_final = MatrixMultiply(m_final, MatrixTranslate(t_final.x, t_final.y, t_final.z));

	return m_final;
 }

//returns a vector that is perpendicular to v,
//there are many but we do not care
Vector3 get_perp_vec(const Vector3 v) {
	Vector3 c;
	if (v.y != 0.0f || v.z != 0.0f)
		c = { 1, 0, 0 };
	else
		c = { 0, 1, 0 };
	return Vector3CrossProduct(v, c);
}

//primitives and collision detection code
struct sphere_t {
	Vector3 c;
	float r;
};

struct plane_t {
    Vector3 n;  // Plane normal. Points x on the plane satisfy Dot(n,x) = d
    float d;   // d = dot(n,p) for a given point p on the plane
};

// Given three noncollinear points (ordered ccw), compute the plane equation
plane_t compute_plane(Vector3 a, Vector3 b, Vector3 c) {
    plane_t p;
    p.n = Vector3Normalize(Vector3CrossProduct(b - a, c - a));
    p.d = Vector3DotProduct(p.n, a);
    return p;
}

//given a normal and position compute plane
plane_t compute_plane(Vector3 n, Vector3 c) {
    plane_t p;
	p.n = n;
    p.d = Vector3DotProduct(p.n, c);
    return p;
}

struct OBB_t {
	Vector3 c; //Center point
	Vector3 u[3]; // local x, y, z axes
	float e[3];    //Positive halfwidth extents of OBB along each axis 

	void init(const Vector3 c_in, const Vector3 x_in, const Vector3 y_in, const Vector3 z_in, const Vector3 hlf_extents_in) {
		c = c_in;
		u[0] = x_in;
		u[1] = y_in;
		u[2] = z_in;
		e[0] = hlf_extents_in.x;
		e[1] = hlf_extents_in.y;
		e[2] = hlf_extents_in.z;
	}
};

void convert_OBB_to_polygon(const OBB_t& o, Vector3 (*out_pts)[8]) {
	Vector3 x_half = o.u[0]*o.e[0];
	Vector3 y_half = o.u[1]*o.e[1];
	Vector3 z_half = o.u[2]*o.e[2];

	(*out_pts)[0] = o.c + x_half + y_half + z_half;
	(*out_pts)[1] = o.c + x_half + y_half - z_half;
	(*out_pts)[2] = o.c + x_half - y_half + z_half;
	(*out_pts)[3] = o.c + x_half - y_half - z_half;
	(*out_pts)[4] = o.c - x_half + y_half + z_half;
	(*out_pts)[5] = o.c - x_half + y_half - z_half;
	(*out_pts)[6] = o.c - x_half - y_half + z_half;
	(*out_pts)[7] = o.c - x_half - y_half - z_half;
	return;
}

void convert_polygon_to_OBB(const Vector3 (*pts)[8], OBB_t* out_obb) {
	Vector3 x_half = ((*pts)[3] - (*pts)[7])*0.5f;
	Vector3 y_half = ((*pts)[5] - (*pts)[7])*0.5f;
	Vector3 z_half = ((*pts)[6] - (*pts)[7])*0.5f;

	Vector3 c = (*pts)[7] + x_half + y_half + z_half;
	Vector3 x_norm = Vector3Normalize(x_half);
	Vector3 y_norm = Vector3Normalize(y_half);
	Vector3 z_norm = Vector3Normalize(z_half);
	Vector3 half_lengths = Vector3{ Vector3Length(x_half), Vector3Length(y_half), Vector3Length(z_half) };

	out_obb->init(c, x_norm , y_norm, z_norm, half_lengths);

	return;
}

void compute_minkowski_diff(const OBB_t o1, const OBB_t o2, Vector3 (*out_pts)[64]) {
	//convert obbs into polygons
	Vector3 p1[8];
	Vector3 p2[8];

	convert_OBB_to_polygon(o1, &p1);
	convert_OBB_to_polygon(o2, &p2);

	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			(*out_pts)[i*8 + j] = p1[i] - p2[j];
		}
	}
	return;
}

float sq_dist_point_AABB(const Vector3 p_m, const BoundingBox& b) {
	float3 p_buf = Vector3ToFloatV(p_m);
	float3 min_buf = Vector3ToFloatV(b.min);
	float3 max_buf = Vector3ToFloatV(b.max);
	float sq_dist = 0.0f;
	for(int i = 0; i < 3; i++) {
		float v = p_buf.v[i];
		if(v < min_buf.v[i]) sq_dist += (min_buf.v[i] - v)*(min_buf.v[i] - v);
		if(v > max_buf.v[i]) sq_dist += (v - max_buf.v[i])*(v - max_buf.v[i]);
	}
	return sq_dist;
}

float sq_dist_point_OBB(const Vector3& p, const OBB_t& b) {
	Vector3 v = p - b.c;
	float sq_dist = 0.0f;
	for(int i = 0; i < 3; i++) {
		float d = Vector3DotProduct(v, b.u[i]), excess = 0.0f;
		if(d < -b.e[i])
			excess = d + b.e[i];
		else if(d > b.e[i])
			excess = d - b.e[i];
		sq_dist += excess*excess;
	}
	return sq_dist;
}

Vector3 closest_point_triangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c) {
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 bc = c - b;

    // Compute parametric position s for projection P' of P on AB,
    // P' = A + s*AB, s = snom/(snom+sdenom)
    float snom = Vector3DotProduct(p - a, ab), sdenom = Vector3DotProduct(p - b, a - b);

    // Compute parametric position t for projection P' of P on AC,
    // P' = A + t*AC, s = tnom/(tnom+tdenom)
    float tnom = Vector3DotProduct(p - a, ac), tdenom = Vector3DotProduct(p - c, a - c);

    if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

    // Compute parametric position u for projection P' of P on BC,
    // P' = B + u*BC, u = unom/(unom+udenom)
    float unom = Vector3DotProduct(p - b, bc), udenom = Vector3DotProduct(p - c, b - c);

    if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
    if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out

    // P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
    Vector3 n = Vector3CrossProduct(b - a, c - a);
    float vc = Vector3DotProduct(n, Vector3CrossProduct(a - p, b - p));
    // If P outside AB and within feature region of AB,
    // return projection of P onto AB
    if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
        return a + snom / (snom + sdenom) * ab;

    // P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
    float va = Vector3DotProduct(n, Vector3CrossProduct(b - p, c - p));
    // If P outside BC and within feature region of BC,
    // return projection of P onto BC
    if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
        return b + unom / (unom + udenom) * bc;

    // P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
    float vb = Vector3DotProduct(n, Vector3CrossProduct(c - p, a - p));
    // If P outside CA and within feature region of CA,
    // return projection of P onto CA
    if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
        return a + tnom / (tnom + tdenom) * ac;

    // P must project inside face region. Compute Q using barycentric coordinates
    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1.0f - u - v; // = vc / (va + vb + vc)
    return u * a + v * b + w * c;
}

bool collision_AABB_sphere(const BoundingBox& b, const sphere_t& s) {
	float sq_dist = sq_dist_point_AABB(s.c, b);
	return sq_dist <= s.r*s.r;
}

bool collision_OBB_sphere(const OBB_t& b, const sphere_t& s) {
	float sq_dist = sq_dist_point_OBB(s.c, b);
	return sq_dist <= s.r*s.r;
}

bool collision_sphere_sphere(const sphere_t& a, const sphere_t& b) {
	Vector3 d = a.c - b.c;
	float d2 = Vector3DotProduct(d, d);
	float radius_sum = a.r + b.r;
	return d2 <= radius_sum*radius_sum;
}

bool collision_AABB_AABB(const BoundingBox& a, const BoundingBox& b) {
	if(a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if(a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if(a.max.z < b.min.z || a.min.z > b.max.z) return false;

	return true;
}

bool collision_OBB_OBB(const OBB_t &a, const OBB_t &b) {
    float ra, rb;
    float R[3][3], AbsR[3][3];

    // Compute rotation matrix expressing b in a's coordinate frame
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = Vector3DotProduct(a.u[i], b.u[j]);

    // Compute translation vector t
    Vector3 t_vec = b.c - a.c;
    // Bring translation into a's coordinate frame
	float t[3] = { Vector3DotProduct(t_vec, a.u[0]), Vector3DotProduct(t_vec, a.u[1]), Vector3DotProduct(t_vec, a.u[2]) };

    // Compute common subexpressions. Add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is (near) null (see text for details)
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            AbsR[i][j] = abs(R[i][j]) + EPSILON;

    // Test axes L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++) {
        ra = a.e[i];
        rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
        if (abs(t[i]) > ra + rb) return 0;
    }

    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++) {
        ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
        rb = b.e[i];
        if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 0;
    }

    // Test axis L = A0 x B0
    ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
    rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];
    if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 0;

    // Test axis L = A0 x B1
    ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
    rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];
    if (abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 0;

    // Test axis L = A0 x B2
    ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
    rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];
    if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 0;

    // Test axis L = A1 x B0
    ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
    rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];
    if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 0;

    // Test axis L = A1 x B1
    ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
    rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];
    if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 0;

    // Test axis L = A1 x B2
    ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
    rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];
    if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 0;

    // Test axis L = A2 x B0
    ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
    rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];
    if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 0;

    // Test axis L = A2 x B1
    ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
    rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];
    if (abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 0;

    // Test axis L = A2 x B2
    ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
    rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];
    if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 0;

    // Since no separating axis found, the OBBs must be intersecting
    return 1;
}

//Computes a aabb around the obb
void convert_OBB_to_AABB(const OBB_t& obb, BoundingBox* out_bb) {
    // Get min and max vertex to construct bounds (AABB)
    Vector3 minVertex = { 0 };
    Vector3 maxVertex = { 0 };

	//compute 8 obb verts
	Vector3 pts[8];
	convert_OBB_to_polygon(obb, &pts);

	minVertex = pts[0];
	maxVertex = pts[0];

	for(int i = 1; i < 8; i++)
	{
		minVertex = Vector3Min(minVertex, pts[i]);
		maxVertex = Vector3Max(maxVertex, pts[i]);
	}

    // Create the bounding box
    out_bb->min = minVertex;
    out_bb->max = maxVertex;
}

void convert_AABB_to_OBB(const BoundingBox& bb, OBB_t* out_obb) {
	Vector3 half_extents = (bb.max - bb.min)*0.5f;
	out_obb->init(bb.min + half_extents, UNIT_VECS[DIR_X], UNIT_VECS[DIR_Y], UNIT_VECS[DIR_Z], half_extents);
	return;
}

//this just converts the aabb to an obb and calls obb vs obb
bool collision_AABB_OBB(const BoundingBox& bb, const OBB_t& obb) {
	OBB_t new_obb;
	convert_AABB_to_OBB(bb, &new_obb);

	return collision_OBB_OBB(new_obb, obb);
}

//intersection functions (returns intersection info)
struct intersection_info_t {
	bool colliding = false;
	Vector3 intersec_vector_m = Vector3Zero();
};

// Takes in 2 AABB and checks if they are colliding, if they are, 
// return the vec they are colliding by, and the intersection length.
intersection_info_t intersection_AABB(const BoundingBox& a, const BoundingBox& b) {
	intersection_info_t c;
	c.colliding = false;
	c.intersec_vector_m = Vector3Zero();
	Vector3 distance1 = b.min - a.max;
	Vector3 distance2 = a.min - b.max;
	Vector3 distances;

	distances.x = fmax(distance1.x, distance2.x);
	distances.y = fmax(distance1.y, distance2.y);
	distances.z = fmax(distance1.z, distance2.z);

	float distance = fmax(distances.x, fmax(distances.y, distances.z));

	if(distance < 0.0f) {
		c.colliding = true;
		if(distances.x == distance) {
			if(a.max.x > b.max.x) {
				c.intersec_vector_m.x = distances.x;
			}
			else {
				c.intersec_vector_m.x = -distances.x;
			}
		}
		if(distances.y == distance) {
			if(a.max.y > b.max.y) {
				c.intersec_vector_m.y = distances.y;
			}
			else {
				c.intersec_vector_m.y = -distances.y;
			}
		}
		if(distances.z == distance) {
			if(a.max.z > b.max.z) {
				c.intersec_vector_m.z = distances.z;
			}
			else {
				c.intersec_vector_m.z = -distances.z;
			}
		}
	}
	return c;
}

//used by intersection_OBB_OBB
bool test_axis_and_compare_min_axis(intersection_info_t* out_info, float* out_min, float tdotl_no_abs, Vector3 L_axis, float ra, float rb) {
	float tdotl = abs(tdotl_no_abs);
	//ignore axes that have no length
	if(Vector3LengthSqr(L_axis) > EPSILON) {
		if(ra + rb - tdotl < *out_min) {
			*out_min = ra + rb - tdotl;
			out_info->intersec_vector_m = Vector3Normalize(L_axis*-copysignf(1.0f, tdotl_no_abs))*(*out_min);
		}
	}

	return tdotl > ra + rb;
}

//I have spent so much time on this collision code that I think it is time to move on.
//@BUG:This is not 100% correct, it does not return the correct axis of min seperation, but it could be close enough for now.
intersection_info_t intersection_OBB_OBB(const OBB_t &a, const OBB_t &b) {
	intersection_info_t out;
	float min_overlap = FLT_MAX;
    float ra, rb;
    float R[3][3], AbsR[3][3];

    // Compute rotation matrix expressing b in a's coordinate frame
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = Vector3DotProduct(a.u[i], b.u[j]);

    // Compute translation vector t
    Vector3 t_vec = b.c - a.c;
    // Bring translation into a's coordinate frame
	float t[3] = { Vector3DotProduct(t_vec, a.u[0]), Vector3DotProduct(t_vec, a.u[1]), Vector3DotProduct(t_vec, a.u[2]) };

    // Compute common subexpressions. Add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is (near) null (see text for details)
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            AbsR[i][j] = abs(R[i][j]) + EPSILON;

    // Test axes L = A0, L = A1, L = A2
	for(int i = 0; i < 3; i++) {
        ra = a.e[i];
        rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
		Vector3 L_axis = a.u[i];
	//	debug_drawer.add_line(a.c, a.c + L_axis*ra, GREEN, 0.0f, false);
		//debug_drawer.add_line(b.c, b.c + Vector3RotateByQuaternion(L_axis, q)*rb, GREEN, 0.0f, false);
		bool c = test_axis_and_compare_min_axis(&out, &min_overlap, t[i], a.u[i], ra, rb);
		if(c) return out;
    }

    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++) {
        ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
        rb = b.e[i];
		Vector3 L_axis = b.u[i];
		float tdotl_no_abs = t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i];
		//debug_drawer.add_line(a.c, a.c + L_axis*ra, WHITE, 0.0f, false);
		//debug_drawer.add_line(b.c, b.c + L_axis*rb, WHITE, 0.0f, false);
		bool c = test_axis_and_compare_min_axis(&out, &min_overlap, tdotl_no_abs, b.u[i], ra, rb);
		if(c) return out;
    }
	//out.intersec_vector_m = Vector3RotateByQuaternion(out.intersec_vector_m, q_a);

    // Since no separating axis found, the OBBs must be intersecting
	out.colliding = true;
    return out;
}

//this just converts the aabb to an obb and calls obb vs obb
intersection_info_t intersection_AABB_OBB(const BoundingBox& bb, const OBB_t& obb) {
	OBB_t new_obb;
	convert_AABB_to_OBB(bb, &new_obb);

	return intersection_OBB_OBB(new_obb, obb);
}

intersection_info_t intersection_AABB_sphere(const BoundingBox& b, const Vector3& s_pos, float s_rad) {
	intersection_info_t c;
	c.colliding = false;
	c.intersec_vector_m = Vector3Zero();

	Vector3 half = (b.max - b.min)*0.5;
	Vector3 center = b.min + half;

	const float dx = s_pos.x - center.x;
    const float px = half.x - abs(dx);
    if (px <= 0) {
      return c;
    }

	const float dy= s_pos.y - center.y;
    const float py = half.y - abs(dy);
    if (py <= 0) {
      return c;
    }
	
	const float dz = s_pos.z - center.z;
    const float pz = half.z - abs(dz);
    if (pz <= 0) {
      return c;
    }

	float min_dist = fmin(px, fmin(py, pz));

	if(min_dist > s_rad) {
		return c;
	}
	c.colliding = true;

    if (min_dist == px) {
      const float sx = copysignf(1.0f,dx);
      c.intersec_vector_m.x = px * sx;
    } else if(min_dist == py) {
      const float sy = copysignf(1.0f,dy);
      c.intersec_vector_m.y = py * sy;
	} else {
      const float sz = copysignf(1.0f,dz);
      c.intersec_vector_m.z = pz * sz;
	}
    return c;
}

void closest_points_on_two_lines(const Ray& line1, const Ray& line2, Vector3* out_closest_points) {
    out_closest_points[0] = line1.position;
    out_closest_points[1] = line2.position;

    Vector3 d1 = line1.direction;
    Vector3 d2 = line2.direction;

    float a = Vector3DotProduct(d1, d1);
    float b = Vector3DotProduct(d1, d2);
    float e = Vector3DotProduct(d2, d2);

    float d = a*e - b*b;
    if (d != 0) // If the two lines are not parallel.
    {
        Vector3 r = Vector3(line1.position) - Vector3(line2.position);
        float c = Vector3DotProduct(d1, r);
        float f = Vector3DotProduct(d2, r);

        float s = (b*f - c*e) / d;
        float t = (a*f - b*c) / d;
        out_closest_points[0] = line1.position + line1.direction*s;
        out_closest_points[1] = line2.position + line2.direction*t;
    }
}

Vector3 closest_point_on_line(Ray line, Vector3 point) {
    line.direction = Vector3Normalize(line.direction);
    Vector3 lhs = point - line.position;

    float dotP = Vector3DotProduct(lhs, line.direction);
    return line.position + line.direction * dotP;
}

bool collision_segment_plane(Vector3 a, Vector3 b, plane_t p, float* out_t, Vector3* out_q) {
	Vector3 ab = b - a;
	*out_t = (p.d - Vector3DotProduct(p.n, a)) / Vector3DotProduct(p.n, ab);

	if(*out_t >= 0.0f && *out_t < 1.0f) {
		*out_q = a + *out_t*ab;
		return true;
	}

	return false;
}

bool collision_ray_plane(Ray r, plane_t p, Vector3* out_q) {
	float t;
	return collision_segment_plane(r.position, r.position + r.direction*1000, p, &t, out_q);
}

//Ray prim intersection
Vector3 ray_plane_intersection(Ray line, Vector3 plane_center, Vector3 plane_normal) {
	Vector3 out = Vector3Zero();
	float denom = Vector3DotProduct(plane_normal, line.direction);
	if(abs(denom) > EPSILON)
	{
		float t = Vector3DotProduct((plane_center - line.position), plane_normal) / denom;
		if(t >= 0) return line.position + line.direction*t;
	}
	return out;
}

float ScalarTriple(Vector3 u, Vector3 v, Vector3 w) {
	return Vector3DotProduct(Vector3CrossProduct(u, v), w);
}

// Given line pq and ccw quadrilateral abcd, return whether the line
// pierces the triangle. If so, also return the point r of intersection
bool intersect_line_quad(Vector3 p, Vector3 q, Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3* out_r) {
    Vector3 pq = q - p;
    Vector3 pa = a - p;
    Vector3 pb = b - p;
    Vector3 pc = c - p;
    // Determine which triangle to test against by testing against diagonal first
    Vector3 m = Vector3CrossProduct(pc, pq);
    float v = Vector3DotProduct(pa, m); // ScalarTriple(pq, pa, pc);
    if (v >= 0.0f) {
        // Test intersection against triangle abc
        float u = -Vector3DotProduct(pb, m); // ScalarTriple(pq, pc, pb);
        if (u < 0.0f) return false;
        float w = ScalarTriple(pq, pb, pa);
        if (w < 0.0f) return false;
        // Compute r, r = u*a + v*b + w*c, from barycentric coordinates (u, v, w)
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;
        *out_r = u*a + v*b + w*c;
    } else {
        // Test intersection against triangle dac
        Vector3 pd = d - p;
        float u = Vector3DotProduct(pd, m); // ScalarTriple(pq, pd, pc);
        if (u < 0.0f) return false;
        float w = ScalarTriple(pq, pa, pd);
        if (w < 0.0f) return false;
        v = -v;
        // Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w)
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;
        *out_r = u*a + v*d + w*c;
    }
    return true;
}

bool intersect_ray_quad(Ray r, Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3* out_q) {
	return intersect_line_quad(r.position, r.position + r.direction*1000, a, b, c, d, out_q);
}

bool intersect_ray_AABB(const Ray& r, const BoundingBox& a, float* out_tmin, Vector3* out_q)
{
    *out_tmin = 0.0f;          // set to -FLT_MAX to get first hit on line
    float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

	float3 p = Vector3ToFloatV(r.position);
	float3 d = Vector3ToFloatV(r.direction);

	float3 min = Vector3ToFloatV(a.min);
	float3 max = Vector3ToFloatV(a.max);

    // For all three slabs
    for (int i = 0; i < 3; i++) {
        if (abs(d.v[i]) < EPSILON) {
            // Ray is parallel to slab. No hit if origin not within slab
            if (p.v[i] < min.v[i] || p.v[i] > max.v[i]) return false;
        } else {
            // Compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / d.v[i];
            float t1 = (min.v[i] - p.v[i]) * ood;
            float t2 = (max.v[i] - p.v[i]) * ood;
            // Make t1 be intersection with near plane, t2 with far plane
            if (t1 > t2) std::swap(t1, t2);
            // Compute the intersection of slab intersections intervals
            *out_tmin = fmax(*out_tmin, t1);
            tmax = fmin(tmax, t2);
            // Exit with no collision as soon as slab intersection becomes empty
            if (*out_tmin > tmax) return false;
        }
    }
    // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin) 
    *out_q = r.position + r.direction*(*out_tmin);
    return true;
}

#define CMP(x, y) \
	(fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

bool intersect_ray_OBB(const Ray& ray, const OBB_t& obb, float* out_t, Vector3* out_q) {
	Vector3 p = obb.c - ray.position;

	Vector3 X = obb.u[0];
	Vector3 Y = obb.u[1];
	Vector3 Z = obb.u[2];

	Vector3 f = Vector3{
		Vector3DotProduct(X, ray.direction),
		Vector3DotProduct(Y, ray.direction),
		Vector3DotProduct(Z, ray.direction)
	};

	Vector3 e = Vector3{
		Vector3DotProduct(X, p),
		Vector3DotProduct(Y, p),
		Vector3DotProduct(Z, p)
	};

	// The above loop simplifies the below if statements
	// this is done to make sure the sample fits into the book
	if (CMP(f.x, 0)) {
		if (-e.x - obb.e[0] > 0 || -e.x + obb.e[0] < 0) {
			return -1;
		}
		f.x = 0.00001f; // Avoid div by 0!
	}
	else if (CMP(f.y, 0)) {
		if (-e.y - obb.e[1] > 0 || -e.y + obb.e[1] < 0) {
			return -1;
		}
		f.y = 0.00001f; // Avoid div by 0!
	}
	else if (CMP(f.z, 0)) {
		if (-e.z - obb.e[2] > 0 || -e.z + obb.e[2] < 0) {
			return -1;
		}
		f.z = 0.00001f; // Avoid div by 0!
	}
	float t1 = (e.x + obb.e[0]) / f.x;
	float t2 = (e.x - obb.e[0]) / f.x;
	float t3 = (e.y + obb.e[1]) / f.y;
	float t4 = (e.y - obb.e[1]) / f.y;
	float t5 = (e.z + obb.e[2]) / f.z;
	float t6 = (e.z - obb.e[2]) / f.z;
	float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
	float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

	// if tmax < 0, ray is intersecting AABB
	// but entire AABB is behind it's origin
	if (tmax < 0) {
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		return false;
	}

	// If tmin is < 0, tmax is closer
	float t_result = tmin;

	if (tmin < 0.0f) {
		t_result = tmax;
	}

	//outResult->hit = true;
	//outResult->t = t_result;
	*out_t = t_result;
	*out_q = ray.position + ray.direction * t_result;

	Vector3 normals[] = {
			X,			// +x
			X * -1.0f,	// -x
			Y,			// +y
			Y * -1.0f,	// -y
			Z,			// +z
			Z * -1.0f	// -z
	};
#if 0
	for(int i = 0; i < 6; ++i) {
		if(CMP(t_result, t[i])) {
			outResult->normal = Vector3Normalize(normals[i]);
		}
	}
#endif
	return true;
}

//Game types
enum INPUT_FLAGS {
	W    = 1 << 0,  // 0000 0000 0000 0001 
	A   = 1 << 1,  // 0000 0000 0000 0010
	S    = 1 << 2,  // 0000 0000 0000 0100
	D   = 1 << 3,  // 0000 0000 0000 1000
	SPACE    = 1 << 4,  // 0000 0000 0001 0000
	LEFT_SHIFT   = 1 << 5,  // 0000 0000 0010 0000
	LMB = 1 << 6,  // 0000 0000 0100 0000
	R = 1 << 7,  // 0000 0000 1000 0000
	EXIT    = 1 << 8,  // 0000 0001 0000 0000
	MMB   = 1 << 9,  // 0000 0010 0000 0000
	RMB    = 1 << 10, // 0000 0100 0000 0000
	ROT      = 1 << 11, // 0000 1000 0000 0000
	AABBCOMP = 1 << 12, // 0001 0000 0000 0000
	ENTER = 1 << 13,
	RIGHT      = 1 << 14,   // 0100 0000 0000 0000
	LEFT      = 1 << 15,   // 0100 0000 0000 0000
	P = 1 << 16,
	Q = 1 << 17,
	E = 1 << 18,
	Z = 1 << 19,
	T = 1 << 20,
};

struct input_state_t {
	uint32_t input_state; //current button state
	uint32_t prev_input_state; //prev button state

	uint32_t input_down; //inputs that just went down
	uint32_t input_up; //inputs that just went up

	Vector2 mouse_pos; //Normalized screen coords (0,0) is center of screen

	void generate_input_events() {
		uint32_t input_changes = input_state ^ prev_input_state;
		
		input_down = (input_changes & input_state) | input_down;

		input_up = (input_changes & (~input_state)) | input_up;
	}

	void consume_input() {
		input_down = 0;
		input_up = 0;
	}
};

struct object_id_manager_t {
	std::unordered_map<int, void*> game_object_ptrs;
	int object_id_i = 0;

	int gen_id() {
		int out_id = object_id_i;
		object_id_i = (object_id_i + 1) % OBJ_ID_ROLLOVER;
		return out_id;
	}

	void add_object(int ID, void* obj_ptr) {
		game_object_ptrs.insert(std::make_pair(ID, obj_ptr));
	}

	void remove_object(int ID) {
		game_object_ptrs.erase(ID);
	}

	//tries to get an object assosiated with the ID, if it does not exist 
	//return null
	void* get_object_from_id(int ID) {

		auto it = game_object_ptrs.find(ID);
		if(it != game_object_ptrs.end()) {
			return (*it).second;
		}
		return null;
	}
};

struct timer_t {
	float accum_s = 0.0f;
	//per second
	float freq_hz = 1.0f;

	void reset() {
		accum_s = 1.0f/freq_hz;
	}

	bool tick(float dts) {
		accum_s -= dts;
		if(accum_s < EPSILON) {
			return true;
		}
		return false;
	}
};

enum WIDGET { WIDGET_TRANSLATE, WIDGET_ROTATE, WIDGET_SCALE };
enum class OBJECT_TYPE{TYPE_NULL, TYPE_PLAYER, TYPE_ENEMY, TYPE_LEVEL_GEO, TYPE_BULLET, TYPE_SPAWNER, TYPE_TEXT_TRIGGER, TYPE_ENEMY_BUFF_VOL};

enum COLLISION_TYPE {
	COL_NULL = 1 << 0,
	COL_FRIEND_BULLET = 1 << 1,
	COL_PLAYER = 1 << 2,
	COL_ENEMY = 1 << 3,
	COL_LEVEL_GEO = 1 << 4,
	COL_FRIEND_SHIELD = 1 << 5,
	COL_BOTTOM_BB = 1 << 6,
	COL_ENEMY_BULLET = 1 << 7,
	COL_ENEMY_MELEE = 1 << 8,
	COL_PLAYER_MELEE = 1 << 9,
	COL_SPAWN_TRIGGER = 1 << 10,
	COL_PLAYER_TRIGGER = 1 << 11,
	COL_TEXT_TRIGGER = 1 << 12,
	COL_ENEMY_BUFF_VOL = 1 << 13,
	COL_MAX = 1 << 13,
};

#define LETTER_BOUNDRY_SIZE     0.25f
#define TEXT_MAX_LAYERS         32
#define LETTER_BOUNDRY_COLOR    VIOLET
bool SHOW_LETTER_BOUNDRY = false;
//Drawing related functions
//--------------------------------------------------------------------------------------
// Module Functions Definitions
//--------------------------------------------------------------------------------------
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
	// Character index position in sprite font
	// NOTE: In case a codepoint is not available in the font, index returned points to '?'
	int index = GetGlyphIndex(font, codepoint);
	float scale = fontSize / (float)font.baseSize;

	// Character destination rectangle on screen
	// NOTE: We consider charsPadding on drawing
	position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding) / (float)font.baseSize * scale;
	position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding) / (float)font.baseSize * scale;

	// Character source rectangle from font texture atlas
	// NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
	Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
											 font.recs[index].width + 2.0f * font.glyphPadding, font.recs[index].height + 2.0f * font.glyphPadding };

	float width = (float)(font.recs[index].width + 2.0f * font.glyphPadding) / (float)font.baseSize * scale;
	float height = (float)(font.recs[index].height + 2.0f * font.glyphPadding) / (float)font.baseSize * scale;

	if (font.texture.id > 0)
	{
		const float x = 0.0f;
		const float y = 0.0f;
		const float z = 0.0f;

		// normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
		const float tx = srcRec.x / font.texture.width;
		const float ty = srcRec.y / font.texture.height;
		const float tw = (srcRec.x + srcRec.width) / font.texture.width;
		const float th = (srcRec.y + srcRec.height) / font.texture.height;

		if (SHOW_LETTER_BOUNDRY) DrawCubeWiresV(Vector3 { position.x + width / 2, position.y, position.z + height / 2 }, Vector3 { width, LETTER_BOUNDRY_SIZE, height }, LETTER_BOUNDRY_COLOR);

		rlCheckRenderBatchLimit(4 + 4 * backface);
		rlSetTexture(font.texture.id);

		rlPushMatrix();
		rlTranslatef(position.x, position.y, position.z);

		rlBegin(RL_QUADS);
		rlColor4ub(tint.r, tint.g, tint.b, tint.a);

		// Front Face
		rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
		rlTexCoord2f(tx, ty); rlVertex3f(x, y, z);              // Top Left Of The Texture and Quad
		rlTexCoord2f(tx, th); rlVertex3f(x, y, z + height);     // Bottom Left Of The Texture and Quad
		rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
		rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

		if (backface)
		{
			// Back Face
			rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
			rlTexCoord2f(tx, ty); rlVertex3f(x, y, z);          // Top Right Of The Texture and Quad
			rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
			rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
			rlTexCoord2f(tx, th); rlVertex3f(x, y, z + height); // Bottom Right Of The Texture and Quad
		}
		rlEnd();
		rlPopMatrix();

		rlSetTexture(0);
	}
}

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char* text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint)
{
	int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

	float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
	float textOffsetX = 0.0f;               // Offset X to next character to draw

	float scale = fontSize / (float)font.baseSize;

	for (int i = 0; i < length;)
	{
		// Get next codepoint from byte string and glyph index in font
		int codepointByteCount = 0;
		int codepoint = GetCodepoint(&text[i], &codepointByteCount);
		int index = GetGlyphIndex(font, codepoint);

		// NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
		// but we need to draw all of the bad bytes using the '?' symbol moving one byte
		if (codepoint == 0x3f) codepointByteCount = 1;

		if (codepoint == '\n')
		{
			// NOTE: Fixed line spacing of 1.5 line-height
			// TODO: Support custom line spacing defined by user
			textOffsetY += scale + lineSpacing / (float)font.baseSize * scale;
			textOffsetX = 0.0f;
		}
		else
		{
			if ((codepoint != ' ') && (codepoint != '\t'))
			{
				DrawTextCodepoint3D(font, codepoint, Vector3 { position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, backface, tint);
			}

			if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing) / (float)font.baseSize * scale;
			else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing) / (float)font.baseSize * scale;
		}

		i += codepointByteCount;   // Move text bytes counter to next codepoint
	}
}

// Measure a text in 3D. For some reason `MeasureTextEx()` just doesn't seem to work so i had to use this instead.
static Vector3 MeasureText3D(Font font, const char* text, float fontSize, float fontSpacing, float lineSpacing)
{
	int len = TextLength(text);
	int tempLen = 0;                // Used to count longer text line num chars
	int lenCounter = 0;

	float tempTextWidth = 0.0f;     // Used to count longer text line width

	float scale = fontSize / (float)font.baseSize;
	float textHeight = scale;
	float textWidth = 0.0f;

	int letter = 0;                 // Current character
	int index = 0;                  // Index position in sprite font

	for (int i = 0; i < len; i++)
	{
		lenCounter++;

		int next = 0;
		letter = GetCodepoint(&text[i], &next);
		index = GetGlyphIndex(font, letter);

		// NOTE: normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
		// but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
		if (letter == 0x3f) next = 1;
		i += next - 1;

		if (letter != '\n')
		{
			if (font.glyphs[index].advanceX != 0) textWidth += (font.glyphs[index].advanceX + fontSpacing) / (float)font.baseSize * scale;
			else textWidth += (font.recs[index].width + font.glyphs[index].offsetX) / (float)font.baseSize * scale;
		}
		else
		{
			if (tempTextWidth < textWidth) tempTextWidth = textWidth;
			lenCounter = 0;
			textWidth = 0.0f;
			textHeight += scale + lineSpacing / (float)font.baseSize * scale;
		}

		if (tempLen < lenCounter) tempLen = lenCounter;
	}

	if (tempTextWidth < textWidth) tempTextWidth = textWidth;

	Vector3 vec = { 0 };
	vec.x = tempTextWidth + (float)((tempLen - 1) * fontSpacing / (float)font.baseSize * scale); // Adds chars spacing to measure
	vec.y = 0.25f;
	vec.z = textHeight;

	return vec;
}
