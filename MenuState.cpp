#include "MenuState.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <graphics/Scene.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include <maths/math_utils.h>
#include <math.h>
#include <graphics/renderer_3d.h>
#include "ar_app.h"


MenuState::MenuState(gef::Platform& platform) :
	State(platform),
	model_scene_(NULL),
	mesh_(NULL),
	model_scene_2_(NULL),
	mesh_2_(NULL)
{
}

void MenuState::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager)
{
	//input_manager_ = gef::InputManager::Create(platform_);
}

void MenuState::Release(gef::AudioManager* audio_manager)
{
	//CleanUpFont();

	delete model_scene_;
	model_scene_ = NULL;

	delete mesh_;
	mesh_ = NULL;

	delete model_scene_2_;
	model_scene_ = NULL;

	delete mesh_2_;
	mesh_ = NULL;

}
void MenuState::Update(float frame_time, gef::InputManager * inputManager, State ** nextState)
{
	fps_ = 1.0f / frame_time;

	// read input devices
	if (inputManager) {
		inputManager->Update();
		// controller input
		gef::SonyControllerInputManager* controller_manager = inputManager->controller_input();
		if (controller_manager) {
			const gef::SonyController* controller = controller_manager->GetController(0);
			if (controller) {
				if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {
					//audio_manager_->PlaySample(buttonSelect_sfx_id);
					/// switch to control what happens when the cross button is pressed
					/// button input sets the state based on what has been selected
					ARApp::SetState(new GameState(platform_));
				}
			}

			// keyboard input
			//gef::Keyboard* keyboard = inputManager->keyboard();
			//if (keyboard)
			//{
			//	float accelleration = 25.f;
			//	if (keyboard->IsKeyDown(gef::Keyboard::KC_W)) {
			//		//player_.velocity_ += gef::Vector4(0.f, accelleration, 0.f) * frame_time;
			//	}
			//	if (keyboard->IsKeyDown(gef::Keyboard::KC_A)) {
			//		//player_.velocity_ += gef::Vector4(-accelleration, 0.f, 0.f) * frame_time;
			//	}
			//	if (keyboard->IsKeyDown(gef::Keyboard::KC_S)) {
			//		//player_.velocity_ += gef::Vector4(0.f, -accelleration, 0.f) * frame_time;
			//	}
			//	if (keyboard->IsKeyDown(gef::Keyboard::KC_D)) {
			//		//player_.velocity_ += gef::Vector4(accelleration, 0.f, 0.f) * frame_time;
			//	}
			//}
		}
	}
}

void MenuState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();
	DrawHUD(spriteRenderer, font, width, height);
	spriteRenderer->End();
}

void MenuState::DrawHUD(gef::SpriteRenderer * spriteRenderer, gef::Font * font, float width, float height)
{
	if (font)
	{
		// display frame rate
		font->RenderText(spriteRenderer, gef::Vector4(width / 2, height / 2, -0.9f), 1.f, 0xffffffff, gef::TJ_LEFT, "MENU");

		font->RenderText(spriteRenderer, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}
