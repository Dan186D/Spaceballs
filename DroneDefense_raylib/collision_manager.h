#pragma once
#include "pch.h"
#include "common.h"

//this collision system has a lot of potential problems, first is that it will
//probably not support dynamic objects ie if we have an enemy and that enemy
//collides with a bullet and is killed and deleted, but still has more collisions
//to be resolved, this risks refing a null

//maybe we should have a system that uses an id table to point to game objects

//collision primitive types
enum class PRIM_TYPE {
	PRIM_NULL,
	PRIM_SPHERE,
	PRIM_AABB,
	PRIM_OBB,
};

union primitive_union_t {
	sphere_t sph;
	OBB_t obb;
	BoundingBox aabb;
};

struct primitive_t {
	PRIM_TYPE prim_type = PRIM_TYPE::PRIM_NULL;
	primitive_union_t prim = { 0 };
};

bool primitive_type_check(const primitive_t& prim, PRIM_TYPE t) {
	if(prim.prim_type == t) {
		return true;
	}
	return false;
}

//does a type check on 2 prims, if they are not in the order of 
//the passed arguments swap them
bool type_check_primitives(primitive_t* in_out_a, primitive_t* in_out_b, PRIM_TYPE t1, PRIM_TYPE t2) {
	
	//check if a is type t1
	if(primitive_type_check(*in_out_a, t1)) {
		if(primitive_type_check(*in_out_b, t2)) {
			return true;
		}
		return false;
	}
	
	//check if b is type t1
	if(primitive_type_check(*in_out_b, t1)) {
		if(primitive_type_check(*in_out_a, t2)) {
			swap(*in_out_a, *in_out_b);
			return true;
		}
		return false;
	}

	return false;
}

//returns true if there is a collision between the 2 passed prims
bool collision_primitives(const primitive_t& a_in, const primitive_t& b_in) {
	primitive_t a = a_in;
	primitive_t b = b_in;

	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_AABB, PRIM_TYPE::PRIM_AABB)) {
		BoundingBox* bb_a = (BoundingBox*)&a.prim;
		BoundingBox* bb_b = (BoundingBox*)&b.prim;
		return collision_AABB_AABB(*bb_a, *bb_b);
	}

	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_SPHERE, PRIM_TYPE::PRIM_AABB)) {
		sphere_t* s_a = (sphere_t*)&a.prim;
		BoundingBox* bb_b = (BoundingBox*)&b.prim;
		return collision_AABB_sphere(*bb_b, *s_a);
	}

	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_SPHERE, PRIM_TYPE::PRIM_SPHERE)) {
		sphere_t* s_a = (sphere_t*)&a.prim;
		sphere_t* s_b = (sphere_t*)&b.prim;
		return collision_sphere_sphere(*s_a, *s_b);
	}

	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_AABB, PRIM_TYPE::PRIM_OBB)) {
		BoundingBox* b_a = (BoundingBox*)&a.prim;
		OBB_t* o_b = (OBB_t*)&b.prim;
#if 0
		OBB_t test;
		convert_AABB_to_OBB(*b_a, &test);
		debug_drawer.add_OBB(*o_b, GREEN);
		debug_drawer.add_OBB(test, PURPLE);
		debug_drawer.add_AABB(*b_a, GREEN);
#endif
		return collision_AABB_OBB(*b_a, *o_b);
	}

#if 0
	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_OBB, PRIM_TYPE::PRIM_OBB)) {
		OBB_t* o_a = (OBB_t*)&a.prim;
		OBB_t* o_b = (OBB_t*)&b.prim;
		return collision_OBB_OBB(*o_a, *o_b);
	}
#endif

	if(type_check_primitives(&a, &b, PRIM_TYPE::PRIM_SPHERE, PRIM_TYPE::PRIM_OBB)) {
		sphere_t* s_a = (sphere_t*)&a.prim;
		OBB_t* o_b = (OBB_t*)&b.prim;
		return collision_OBB_sphere(*o_b, *s_a);
	}

	//Unhandled prim collision
	assert(false);
	MessageBoxA(0, "AHHH" , "FAIL", 0);
	return false;
}

struct collidable_t {
	BoundingBox b = { 0 };
	void* owner = null;
	OBJECT_TYPE owner_type = OBJECT_TYPE::TYPE_NULL;
	primitive_t prim;
	COLLISION_TYPE col_type = COL_NULL;
	uint16_t collides_with = 0;

	void init(BoundingBox b_in, void* owner_in, OBJECT_TYPE owner_type_in, primitive_t prim_in, COLLISION_TYPE c_type_in, uint16_t collides_with_in) {
		//ensure we can store all our flags
		assert(1 << sizeof(collides_with_in)*8 > COLLISION_TYPE::COL_MAX);
		b = b_in;
		owner = owner_in;
		owner_type = owner_type_in;
		prim = prim_in;
		col_type = c_type_in;
		collides_with = collides_with_in;
	}
};

bool collidable_type_check(const collidable_t* ptr, OBJECT_TYPE t) {
	if(ptr->owner_type == t) {
		return true;
	}
	return false;
}

//takes 2 collidables and checks if the owner types match the passed types,
//if they do: return true and put ptrs in out vars in order of passed object types
//otherwise puts the collidables in same order of their corrisponding object types
bool type_check_collidables(collidable_t** in_out_a, collidable_t** in_out_b, OBJECT_TYPE t1, OBJECT_TYPE t2, void** out_t1, void** out_t2) {
	
	//check if a is type t1
	if(collidable_type_check(*in_out_a, t1)) {
		if(collidable_type_check(*in_out_b, t2)) {
			*out_t1 = (*in_out_a)->owner;
			*out_t2 = (*in_out_b)->owner;
			return true;
		}
		return false;
	}
	
	//check if b is type t1
	if(collidable_type_check(*in_out_b, t1)) {
		if(collidable_type_check(*in_out_a, t2)) {
			swap(**in_out_a, **in_out_b);
			*out_t1 = (*in_out_a)->owner;
			*out_t2 = (*in_out_b)->owner;
			return true;
		}
		return false;
	}

	return false;
}

struct collision_info_t {
	collidable_t a;
	collidable_t b;

	void init(collidable_t a_in, collidable_t b_in) {
		a = a_in;
		b = b_in;
	}
};

struct collision_manager_t {
	vector<collidable_t> collidables;

	//have an assigned function pointer that is called every time an overlap is detected, reporting the 2 objects that were overlapping
	void (*on_overlap)(collision_info_t) = null;

	void clear_collidables() {
		collidables.clear();
	}

	void add_collidable(void* owner, BoundingBox b, OBJECT_TYPE owner_type, primitive_t prim_in, COLLISION_TYPE col_type, uint16_t collides_with) {
		collidable_t c;
		c.init(b, owner, owner_type, prim_in, col_type, collides_with);
		collidables.push_back(c);
	}

	void check_world_collisions() {
		//sort the collidable bounding boxes by z axis
		std::sort(collidables.begin(), collidables.end(), [](const collidable_t& a, const collidable_t& b) { return a.b.min.z < b.b.min.z; });

		//check the overlapping intervals and add any to a possible collisions list
		vector<collision_info_t> possible_collisions;
		
		vector<collidable_t*> active_list;
		for(int i = 0; i < collidables.size(); i++) {
			if(debug_collision_detection) {
				debug_drawer.add_AABB(collidables[i].b, BLUE, 0.0f, true);
			}
			collidable_t* col = &collidables[i];
			for(int j = 0; j < active_list.size(); j++) {
				collidable_t* act_col = active_list[j];
				if(act_col->b.max.z < col->b.min.z) {
					active_list.erase(active_list.begin() + j);
					j--;
				}
				else {
					//check collision filters
					bool in_filter = col->col_type & act_col->collides_with || act_col->col_type & col->collides_with;
					if(in_filter) {
						collision_info_t c;
						c.init(*col, *act_col);
						possible_collisions.push_back(c);
					}
				}
			}
			active_list.push_back(col);
		}
		
		//check possible collision list with an apropriate function
		for(int i = 0; i < possible_collisions.size(); i++) {
			const collision_info_t info = possible_collisions[i];

			if(debug_collision_detection) {
				debug_drawer.add_AABB(info.a.b, ORANGE, 0.0f, true);
				debug_drawer.add_AABB(info.b.b, ORANGE, 0.0f, true);
			}

			//Check collision of actual primitives
			if(collision_primitives(info.a.prim, info.b.prim)) {
				if(debug_collision_detection) {
					debug_drawer.add_AABB(info.a.b, PURPLE);
					debug_drawer.add_AABB(info.b.b, PURPLE);
				}
				on_overlap(info);
			}
		}
	}

	//cast ray into world and return closest object of passed type 
	bool cast_ray(const Ray& r, OBJECT_TYPE type, float* out_tmin, Vector3* out_q) {
		float tmin = FLT_MAX;
		float maybe_tmin = FLT_MAX;
		Vector3 q;

		for(int i = 0; i < collidables.size(); i++) {
			const collidable_t* c = &collidables[i];
			if(c->owner_type == type) {
				switch(c->prim.prim_type)
				{
				case PRIM_TYPE::PRIM_AABB:
					intersect_ray_AABB(r, *((BoundingBox*)&c->prim.prim), &maybe_tmin, &q);

					if(maybe_tmin < tmin) {
						tmin = maybe_tmin;
						*out_q = q;
					}
					break;
				case PRIM_TYPE::PRIM_OBB:
					if(intersect_ray_OBB(r, *((OBB_t*)&c->prim.prim), &maybe_tmin, &q)) {
						if(maybe_tmin < tmin) {
							tmin = maybe_tmin;
							*out_q = q;
						}
					}
					break;
				default:
					//No check for this raycast prim
					assert(false);
					break;
				}
			}
		}
		if(tmin != FLT_MAX) {
			*out_tmin = tmin;
			return true;
		}
		return false;
	}

	//cast volume into world and report back overlaps
};


