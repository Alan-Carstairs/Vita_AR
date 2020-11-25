#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <audio/audio_manager.h>
#include "graphics/scene.h"
#include "State.h"
#include "GameState.h"
//#include "Enemy_Manager.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
	///static function used to set the current state
	static void SetState(State *newState);
	///static functions used to get and set the score
	static void SetScore(int currentScore) { score = currentScore; }
	static int getScore() { return score; }

private:
	void InitFont();
	void CleanUpFont();
	///static state pointer used to call the relevant init, render, update and release functions
	static State * currentState;
	static int score;
	static gef::InputManager* input_manager_;
	static gef::AudioManager* audio_manager;
	gef::Renderer3D* renderer;
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	PrimitiveBuilder* primitive_builder_;
};



#endif // _RENDER_TARGET_APP_H