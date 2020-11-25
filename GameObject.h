#pragma once
#include <graphics/mesh_instance.h>

class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	void Init();
	bool Update(float);
	void BuildTransform();
	void BuildLocalTransform();

	float y_rotation_rads;
	gef::Matrix44 local_scale_matrix;
	gef::Matrix44 local_rotate_matrix;
	gef::Matrix44 local_transform;
	gef::Matrix44 local_position;
	gef::Vector4 scale_vector;


	gef::Vector4 position_;
	
	gef::Vector4 scale_;
	
	gef::Vector4 velocity_;

};