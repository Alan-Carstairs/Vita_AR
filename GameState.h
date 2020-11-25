#pragma once

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "GameObject.h"
#include "State.h"
#include "primitive_builder.h"
#include "Enemy.h"
#include "Tower.h"
#include "Projectiles.h"

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class Font;
	class SpriteRenderer;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
	class Scene;
}


class GameState : public State
{
public:

	GameState(gef::Platform& p);

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Release(gef::AudioManager* audio_manager);
	bool IsCollidingAABB(const gef::MeshInstance& meshInstance1, const gef::MeshInstance meshInstance2);

	char * FileReader(const char * filename);
	void LoadMapFromFile(const char * filename);

private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD(gef::SpriteRenderer * spriteRenderer);
	void RenderOverlay(gef::SpriteRenderer * spriteRenderer);
	void SetupLights();


	gef::Mesh* GetFirstMesh(gef::Scene* scene);
	void ReadSceneAndAssignFirstMesh(const char* filename, gef::Scene** scene, gef::Mesh** mesh);
	gef::Scene* model_scene_;
	gef::Mesh* mesh_;

	gef::SpriteRenderer* sprite_renderer_;
	gef::InputManager* input_manager_;

	gef::Font* font_;

	float fps_;

	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	gef::Sprite cameraFeedSprite_;
	SceGxmTexture cameraFeed_;
	gef::TextureVita cameraFeedTexture_;
	float cameraImageAspectRatio_, displayAspectRatio_, cameraImageScaleFactor_;
	bool isColliding;

	Tower tower_;
	Projectiles projectiles_;
	GameObject map_;
	Enemy Moving_enemy;
	
	/*gef::MeshInstance cube;
	gef::MeshInstance AR_Cube;*/
	int marker_id;
	gef::Vector4 scale_;

	float timer;


	int num_waypoints;
	GameObject waypoints[15];
	gef::Vector4 position_;
	gef::Vector4 velocity_;
	std::vector<gef::Vector4> waypoints_;
};
