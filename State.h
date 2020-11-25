#pragma once
#include <input/sony_controller_input_manager.h>
#include <graphics/renderer_3d.h>
#include <graphics/font.h>
#include <graphics/sprite.h>
#include "load_texture.h"
#include <audio/audio_manager.h>
#include <input/input_manager.h>
class State
{


public:

	enum GAMESTATE { MENU, PLAYLEVEL, CONTROLS, WIN, LOSE };

	
	GAMESTATE state_;
	State(gef::Platform &p);
	/// virtual functions to be overiddenby child classes
	virtual void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager) {}
	virtual void Update(float frame_time, gef::InputManager * inputManager, State ** nextState) {}
	virtual void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font) {}
	virtual void HandleInput(gef::InputManager* input_manager_) {}
	virtual void Release(gef::AudioManager* audio_manager) {}

	
protected:
	gef::Platform  &platform_;
	
	//gef::AudioManager* audio_manager_;
	//gef::VolumeInfo volInfo;
};