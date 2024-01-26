#pragma once
#include "pch.h"
#include "common.h"

//anything longer than 10 seconds should be streamed

struct sound_t {
	Vector3 src_pos_m = Vector3Zero();
	Sound s = { 0 };
};

//get time length in seconds
float get_sound_time_length(Sound s) {
	float total_seconds = 0.0f;

	total_seconds = (float)s.frameCount/(s.stream.sampleRate*s.stream.channels);

	return total_seconds;
}

#define sound_dir asset_dir "sounds/"

#define sounds \
X(absorb2, .ogg)\
X(laser, .wav)\
X(dirthit, .wav)\
X(Down1, .ogg)\
X(swoosh, .ogg)\
X(newlazer, .wav)\
X(hitsquish, .wav)\
X(unf, .ogg)\
X(evasion2, .ogg)\
X(zombie3, .wav)\
X(poison, .wav)\
X(grapple, .wav)\
X(spawn, .wav)\
X(Magic1, .ogg)\

#define X(name, ext) name,
enum class SOUNDS {
	sounds
	MAX_SOUND,
};
#undef X

#define MAX_SFX 16
#define SOUND_MIN_VOLUME 0.001f
struct audio_manager_t {
	float sound_vol = 1.0f;
	float music_vol = 0.2f;
	sound_t sound_fx[MAX_SFX] = { 0 };
	Music music = { 0 };
	Vector3 listener_pos = Vector3Zero();
	Vector3 listener_look_dir = Vector3Zero();

	void init() {
		InitAudioDevice();
#define X(name, ext) sound_fx[(int)SOUNDS::name].s = LoadSound(sound_dir #name #ext);
		sounds
#undef X

		assert((int)SOUNDS::MAX_SOUND <= MAX_SFX);

		music = LoadMusicStream(sound_dir "ambience.mp3");
		PlayMusicStream(music);
	}

	void destroy() {

	}

	void add_sound_fx(SOUNDS sound, float volume, bool rand_pitch, float pan = 0.5f) {
		sound_t* s = &sound_fx[(int)sound];

		if(volume > SOUND_MIN_VOLUME) {
			if(rand_pitch) {
				SetSoundPitch(s->s, 1.0f + rand_ndc()*0.2f);
			}
			SetSoundPan(s->s, pan);
			SetSoundVolume(s->s, volume);
			PlaySoundMulti(s->s);
		}
	}

	void add_sound_fx_positional(SOUNDS sound, float volume, Vector3 src_pos, bool rand_pitch) {
		sound_t* s = &sound_fx[(int)sound];
		s->src_pos_m = src_pos;
		float distance = Vector3LengthSqr(s->src_pos_m - listener_pos);
		float percived_volume = 1.0f/(0.01f*distance + 1.0f/volume);

		Vector2 look_dir = Vector2Normalize({ listener_look_dir.x, listener_look_dir.z });

		const Vector3 sound_direction = src_pos - player_camera.position;
		const Vector2 sound_direction_2d = Vector2Normalize({ sound_direction.x, sound_direction.z });
		const float theta = Vector2Angle(Vector2{0,1}, look_dir);
		const Vector2 transfromed_vec = Vector2Rotate(sound_direction_2d, -theta);
		float pan = Remap(transfromed_vec.x, -1.0f, 1.0f, 0.0f, 1.0f);

		add_sound_fx(sound, percived_volume, rand_pitch, pan);
	}

	//stretches a sound to make it duration seconds long
	void add_sound_fx_set_duration(SOUNDS sound, float volume, Vector3 src_pos, float duration_s) {
		sound_t* s = &sound_fx[(int)sound];
		float pitch = get_sound_time_length(s->s)/duration_s;

		SetSoundPitch(s->s, pitch);
		add_sound_fx(sound, volume, false);
	}

	void update(Vector3 pos, Vector3 look_dir) {
		listener_pos = pos;
		listener_look_dir = look_dir;
		UpdateMusicStream(music);
		SetMusicVolume(music, music_vol);
	}
};
