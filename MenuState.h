#pragma once
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <vector>
#include "State.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class InputManager;
	class Scene;
}

class MenuState : public State
{
public:
	MenuState(gef::Platform& p);

	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Release(gef::AudioManager* audio_manager);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
private:
	void DrawHUD(gef::SpriteRenderer * spriteRenderer, gef::Font * font, float width, float height);

	float fps_;

	gef::Vector4 camera_eye_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 camera_up_;
	float camera_fov_;
	float near_plane_;
	float far_plane_;

	gef::Scene* model_scene_;
	gef::Mesh* mesh_;

	gef::Scene* model_scene_2_;
	gef::Mesh* mesh_2_;

	bool isColliding;
};


