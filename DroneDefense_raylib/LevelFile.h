#pragma once
#include "pch.h"
#include "io.h"

#include "common.h"

struct level_t {
	char level_filename[CUTE_FILES_MAX_FILENAME] = "New.txt";
	char io_line[1024] = { 0 };
	char io_error[1024] = { 0 };

	Vector3 start_pos = Vector3{ 0.0f, 0.0f, 0.0f };
	float start_pitch = 0;
	float start_yaw = 0;
	BoundingBox end_box = { 0 };

	float fogLevelMin = -100;
	float fogLevelMax = 200;
	Vector3 clearColour = { 0 };
	Vector3 skyCol1 = Vector3{ 0.101961f, 0.619608f, 0.666667f };
	Vector3 skyCol2 = Vector3{0.666667f, 0.666667f, 0.498039f};
	Vector3 skyCol3 = Vector3{ 0.f, 0.f, 0.164706f };
	Vector3 skyCol4 = Vector3{ 0.666667f, 1.f, 1.f };

	//deletes all world geo, models, and empties the level geo vector
	void empty() {
	}

	bool io_level(io_t* io) {
		const uint8_t latest_version = 8;
		uint8_t version = latest_version;
		bool io_success = io_uint8(io, version);

		//load deprecated
		if (version == latest_version - 1) {
			//end

		}
		else if (version == latest_version) {
		}
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
