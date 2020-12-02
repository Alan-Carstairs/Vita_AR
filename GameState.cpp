#include "GameState.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/scene.h>
#include <graphics/render_target.h>
#include <gef.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <system/debug_log.h>
#include <input/sony_controller_input_manager.h>
#include <system/file.h>


GameState::GameState(gef::Platform& platform) :
	State(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	model_scene_(NULL),
	mesh_(NULL)
{
}

void GameState::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager)
{
     input_manager_ = gef::InputManager::Create(platform_);
     //sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
     renderer_3d_ = gef::Renderer3D::Create(platform_);
     primitive_builder_ = new PrimitiveBuilder(platform_);
     
     LoadMapFromFile("waypoints");
     InitFont();
     SetupLights();
     
     // initialise sony framework
     sampleInitialize();
     smartInitialize();
     
     // reset marker tracking
     AppData* dat = sampleUpdateBegin();
     smartTrackingReset();
     sampleUpdateEnd(dat);
     
     cameraImageAspectRatio_ = 640.f / 480.f;
     displayAspectRatio_ = 960.f / 544.f;
     cameraImageScaleFactor_ = displayAspectRatio_ / cameraImageAspectRatio_;
     
     cameraFeedSprite_.set_position(0, 0, 1);
     cameraFeedSprite_.set_width(2.f);
     cameraFeedSprite_.set_height(2.0f * cameraImageScaleFactor_);
     
     position_ = gef::Vector4(0.0f, 0.f, 0.f);
     velocity_ = gef::Vector4(0.f, 0.f, -.1f);
     
     scale_ = gef::Vector4(.25f, .25f, .25f);
     tower_.position_ = gef::Vector4(.0f, .0f, .0f);
     tower_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(.005f, .05f, .005f), tower_.position_));
	

     ReadSceneAndAssignFirstMesh("Map.scn", &model_scene_, &mesh_);
     
     map_.set_mesh(mesh_);
     map_.position_ = gef::Vector4(0.f, .0f, .0f);
     //cube_2.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(.05f, .05f, .05f), cube_2.position_));
     marker_id = 1;
     
     
     timer = 0.f;
     isColliding = false;
     num_waypoints = 15;
     
     for (int i = 0; i < num_waypoints; i++) {
     	//GameObject waypoint;
     	//waypoint.position_ = gef::Vector4(.2f, i * 0.72f, i * .05f);
     	//waypoint.set_mesh(mesh_);
     	waypoints[i].position_ = waypoints_[i];
     	waypoints[i].set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(.05f, .05f, .05f), waypoints[i].position_));
     
     }
     
     Moving_enemy.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(0.5,0.5,0.5), gef::Vector4(0.f, 0.f, 0.f)));
     Moving_enemy.position_ = gef::Vector4(0, 0, 0);
     Moving_enemy.init_Waypoints(waypoints_);
	 Moving_enemy.setup_Waypoints(waypoints_[0], waypoints_[1]);
     //Moving_enemy.setup_Waypoints(waypoints_[Moving_enemy.currentWP_], waypoints_[Moving_enemy.nextWP_]);
     Moving_enemy.reCal_Velocity();
	/* projectiles_.Init(waypoints[1].position_);
	 projectiles_.position_ = gef::Vector4(.05f, .0f, .0f);
	 projectiles_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(.005f, .005f, .005f), projectiles_.position_));*/
     
	//int asdf = 0;
}	//

void GameState::Release(gef::AudioManager* audio_manager)
{
	delete primitive_builder_;
	primitive_builder_ = NULL;

	smartRelease();
	sampleRelease();

	CleanUpFont();

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;
}

void GameState::Update(float frame_time, gef::InputManager * inputManager, State ** nextState)
{
	fps_ = 1.0f / frame_time;


	AppData* dat = sampleUpdateBegin();

	// use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	gef::Matrix44 inverse_marker_transform1;
	gef::Matrix44 inverse_marker_transform2;
	gef::Matrix44 marker_transform1;

	if (sampleIsMarkerFound(0)) {
		// marker is being tracked, get its transform
		sampleGetTransform(0, &marker_transform1);
		// set the transform of the 3D mesh instance to draw on
		// top of the marker
		inverse_marker_transform1.AffineInverse(marker_transform1);

		gef::Matrix44 marker_transform2;
		
		inverse_marker_transform2.AffineInverse(marker_transform2);

		if (sampleIsMarkerFound(1)) 
		{
			sampleGetTransform(1, &marker_transform2);
		}

		
		


		gef::Matrix44 local_marker_transform_2;
		local_marker_transform_2 = marker_transform2 * inverse_marker_transform1;
		
		tower_.Update(frame_time);
		tower_.set_transform(tower_.local_transform * marker_transform2);
		//local_marker_transform_2.SetTranslation(local_marker_transform_2.GetTranslation() / 2);

		//cube_2.position_.Lerp(local_marker_transform_2.GetTranslation(), gef::Vector4(0, 0, 0), timer);
		map_.Update(frame_time);
		map_.set_transform(map_.local_transform * marker_transform1);
		
		//cube.set_transform(cube.local_transform * marker_transform1);
		for (int i = 0; i < num_waypoints; i++) {
			waypoints[i].Update(frame_time);
			waypoints[i].set_transform(waypoints[i].local_transform * marker_transform1);
		}

		projectiles_.Update(frame_time);
		projectiles_.set_transform(projectiles_.local_transform * local_marker_transform_2 * marker_transform1);
		//projectiles_.Cal_Velocity((Moving_enemy.local_transform * marker_transform1 * inverse_marker_transform2).GetTranslation() , tower_.local_transform.GetTranslation());
		//Moving_enemy.position_.Lerp(local_marker_transform_2.GetTranslation(), gef::Vector4(0, 0, 0), timer);
		
		//Moving_enemy.set_transform(Moving_enemy.local_transform * marker_transform1);

	}

	Moving_enemy.Update(frame_time);
	Moving_enemy.set_transform(Moving_enemy.local_transform * marker_transform1);

	if (timer < 1) {
		timer += frame_time / 5;
	}

	isColliding = IsCollidingAABB(tower_, Moving_enemy);
	if (isColliding) {
		velocity_ *= -1;
	}

	if (input_manager_)
	{
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input)
		{
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller)
			{
				if (controller->buttons_pressed() & gef_SONY_CTRL_START) {
					gef::DebugOut("Start was pressed!\n");
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_SELECT) {
					gef::DebugOut("Select was pressed!\n");
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_UP) {
					gef::DebugOut("Up was pressed!\n");
					//game_object_.set_position(sprite_position + gef::Vector4(0, -5, 0));
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_DOWN) {
					gef::DebugOut("Down was pressed!\n");
					//game_object_.set_position(sprite_position + gef::Vector4(0, 5, 0));
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_LEFT) {
					gef::DebugOut("Left was pressed!\n");
					//game_object_.set_position(sprite_position + gef::Vector4(-5, 0, 0));
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_RIGHT) {
					gef::DebugOut("Right was pressed!\n");
					//game_object_.set_position(sprite_position + gef::Vector4(5, 0, 0));
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_L2) {
					gef::DebugOut("Left Bumper was pressed!\n");
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_R2) {
					gef::DebugOut("Right Bumper was pressed!\n");
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE) {
					gef::DebugOut("Triangle was pressed!\n");
					//game_object_.set_position(platform_.width()*0.5f, platform_.height()*0.5f, 0.0f);
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
				{
					projectiles_.Init((Moving_enemy.local_transform * marker_transform1 * inverse_marker_transform2).GetTranslation());
					//projectiles_.position_ = gef::Vector4(.05f, .0f, .0f);
					projectiles_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(.005f, .005f, .005f), projectiles_.position_));
					gef::DebugOut("Circle was pressed!\n");
				}

				if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {
					gef::DebugOut("Cross was pressed!\n");
				}
				if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE) {
					gef::DebugOut("Square was pressed!\n");
				}
				if ((controller->buttons_down() & (gef_SONY_CTRL_SELECT | gef_SONY_CTRL_START)) == (gef_SONY_CTRL_SELECT | gef_SONY_CTRL_START)) {
					gef::DebugOut("Anakin gif!\n");
					//return false;
				}
				/*float left_x_axis = controller->left_stick_x_axis();
				gef::DebugOut("xL: %f\n", left_x_axis);
				float left_y_axis = controller->left_stick_y_axis();
				gef::DebugOut("yL: %f\n", left_y_axis);
				if (left_x_axis < 0) {
				cube.position_.set_x(cube.position_.x() - .01f);
				}
				else if (left_x_axis > 0) {
				cube.position_.set_x(cube.position_.x() + .01f);
				}
				if (left_y_axis < 0) {
				cube.position_.set_y(cube.position_.y() + .01f);
				}
				else if (left_y_axis > 0) {
				cube.position_.set_y(cube.position_.y() - .01f);*/
			}
		}
	}

	sampleUpdateEnd(dat);

	//return true;
}

void GameState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	AppData* dat = sampleRenderBegin();

	gef::Matrix44 OrthographicProjectionMatrix;
	OrthographicProjectionMatrix.SetIdentity();
	OrthographicProjectionMatrix = platform_.OrthographicFrustum(-1, 1, -1, 1, -1, 1);

	// Render the camera feed
	if (dat->currentImage) {
		cameraFeedTexture_.set_texture(dat->currentImage->tex_yuv);
		spriteRenderer->set_projection_matrix(OrthographicProjectionMatrix);
		cameraFeedSprite_.set_texture(&cameraFeedTexture_);
		spriteRenderer->Begin(true);

		spriteRenderer->DrawSprite(cameraFeedSprite_);
		// DRAW CAMERA FEED SPRITE HERE
		spriteRenderer->End();
	}

	//// REMEMBER AND SET THE PROJECTION MATRIX HERE
	gef::Matrix44 ProjectionMatrix;
	ProjectionMatrix.SetIdentity();
	ProjectionMatrix = platform_.PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float)SCE_SMART_IMAGE_WIDTH / (float)SCE_SMART_IMAGE_HEIGHT, .1f, 10);

	gef::Matrix44 scaleMatrix_;
	scaleMatrix_.SetIdentity();
	scaleMatrix_.Scale(gef::Vector4(1, cameraImageScaleFactor_, 1, 1));

	gef::Matrix44 finalProjectionMatrix;
	finalProjectionMatrix.SetIdentity();
	finalProjectionMatrix = ProjectionMatrix * scaleMatrix_;

	gef::Matrix44 ViewMatrix_;
	ViewMatrix_.SetIdentity();

	//
	// Render 3D scene
	//

	/*gef::Matrix44 scale_Matrix;
	scale_Matrix.SetIdentity();
	scale_Matrix.Scale(scale_);
	cube.set_transform(scale_Matrix);*/

	// SET VIEW AND PROJECTION MATRIX HERE
	renderer_3d_->set_projection_matrix(finalProjectionMatrix);
	renderer_3d_->set_view_matrix(ViewMatrix_);


	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	// DRAW 3D MESHES HERE
	renderer_3d_->DrawMesh(projectiles_);
	//renderer_3d_->DrawMesh(tower_);
	renderer_3d_->DrawMesh(map_);
	for (int i = 0; i < num_waypoints; i++) {
		renderer_3d_->DrawMesh(waypoints[i]);
	}
	
	renderer_3d_->DrawMesh(Moving_enemy);
	renderer_3d_->End();

	//RenderOverlay();

	sampleRenderEnd();
}

void GameState::RenderOverlay(gef::SpriteRenderer * spriteRenderer)
{
	//
	// render 2d hud on top
	//
	gef::Matrix44 proj_matrix2d;
	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	spriteRenderer->set_projection_matrix(proj_matrix2d);
	spriteRenderer->Begin(false);
	DrawHUD(spriteRenderer);
	spriteRenderer->End();
}


gef::Mesh* GameState::GetFirstMesh(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
			mesh = scene->CreateMesh(platform_, scene->mesh_data.front());
	}

	return mesh;
}


void GameState::ReadSceneAndAssignFirstMesh(const char* filename, gef::Scene** scene, gef::Mesh** mesh)
{
	gef::Scene* scn = new gef::Scene;
	scn->ReadSceneFromFile(platform_, filename);

	// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file
	scn->CreateMaterials(platform_);

	*mesh = GetFirstMesh(scn);
	*scene = scn;

}

void GameState::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void GameState::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void GameState::DrawHUD(gef::SpriteRenderer * spriteRenderer)
{
	if (font_)
	{
		font_->RenderText(spriteRenderer, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		if (isColliding) {
			font_->RenderText(spriteRenderer, gef::Vector4(850.0f, 380.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Collide");
		}

	}
}

void GameState::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

bool GameState::IsCollidingAABB(const gef::MeshInstance& meshInstance1, const gef::MeshInstance meshInstance2) {

	gef::Aabb aaBB_1 = meshInstance1.mesh()->aabb();
	gef::Aabb aaBB_2 = meshInstance2.mesh()->aabb();

	gef::Aabb aaBB1_Transformed = aaBB_1.Transform(meshInstance1.transform());
	gef::Aabb aaBB2_Transformed = aaBB_2.Transform(meshInstance2.transform());

	if (aaBB1_Transformed.max_vtx().x() > aaBB2_Transformed.min_vtx().x() && aaBB1_Transformed.min_vtx().x() < aaBB2_Transformed.max_vtx().x() &&
		aaBB1_Transformed.max_vtx().y() > aaBB2_Transformed.min_vtx().y() && aaBB1_Transformed.min_vtx().y() < aaBB2_Transformed.max_vtx().y() &&
		aaBB1_Transformed.max_vtx().z() > aaBB2_Transformed.min_vtx().z() && aaBB1_Transformed.min_vtx().z() < aaBB2_Transformed.max_vtx().z()) {

		return true;
	}

	return false;

}
/// function to Read enemy postions from file, takes a file name and concatenates string to a .txt file,
///converts values from file to a char* and returns value called point
char* GameState::FileReader(const char* filename) {

	std::vector<float> values;
	std::string config_filename(filename);
	config_filename += ".txt";
	void* file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();

	bool success = true;
	success = file->Open(config_filename.c_str());
	if (success)
	{
		success = file->GetSize(file_size);
		if (success)
		{
			file_data = malloc(file_size);
			success = file_data != NULL;
			if (success)
			{
				Int32 bytes_read;
				success = file->Read(file_data, file_size, bytes_read);
				if (success) {
					success = bytes_read == file_size;
				}
			}
		}

		char* point = (char*)file_data;

		file->Close();
		delete file;
		file = NULL;

		return point;
	}

	return nullptr;
}

///function to populate vector using FileReader function, creates a vector of floats, converts the char* returned by FileReader function to string
/// loops through the string and converts to float, converts floats to B2vec2 and populates vector of b2Vec2
void GameState::LoadMapFromFile(const char* filename)
{
	char* input = FileReader(filename);

	std::vector<float> positionsValues;
	std::string string = input;
	std::string output;

	for (int i = 0; i < string.size(); i++)
	{
		char currentChar = string[i];


		if (currentChar >= 45 && currentChar <= 57)
		{
			output += currentChar;
		}

		if (currentChar == ',')
		{

			char LE = 13;
			char CR = 10;

			output += LE;
			output += CR;

			float number;
			number = atof(output.c_str());
			positionsValues.push_back(number);
			output.clear();
		}

	}

	for (int i = 0; i < positionsValues.size(); i += 3)
	{
		waypoints_.push_back(gef::Vector4(positionsValues[i], positionsValues[i + 1], positionsValues[i + 2]));
	}

	delete input;
	input = NULL;
}