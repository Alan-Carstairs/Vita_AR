#include "GameObject.h"

GameObject::GameObject()
{
	Init();


}

void GameObject::Init()
{
	position_ = gef::Vector4(0.0f, 0.0f, 0.0f);

	//velocity_ = gef::Vector4(0.0f, 0.0f, 0.0f);

	scale_ = gef::Vector4(4.25f, 4.25f,4.25f);
	y_rotation_rads = 0;
	local_position.SetIdentity();
	local_transform.SetIdentity();
	scale_vector = gef::Vector4(0.05f, .05f, 0.05f);
}

bool GameObject::Update(float Frametime)
{
	//position_ += velocity_ * Frametime;
	BuildTransform();
	//BuildLocalTransform();
	return true;
}

void GameObject::BuildTransform()
{
	//gef::Matrix44 scale;
	//gef::Matrix44 rotX;
	//gef::Matrix44 trans;

	//scale.SetIdentity();
	//scale.Scale(scale_);

	//rotX.SetIdentity();
	//rotX.RotationX(...)

	//trans.SetIdentity();
	//trans.SetTranslation(position_);

	//gef::Matrix44 transformMatrix = scale * trans;

	//set_transform(transformMatrix);
	local_position.SetTranslation(position_);
	local_scale_matrix.Scale(scale_vector);
	//calculate local rotation
	local_rotate_matrix.RotationY(y_rotation_rads);
	//calculate local transform matrix
	local_transform = local_scale_matrix * local_rotate_matrix * local_position;


}
void GameObject::BuildLocalTransform()
{
	local_scale_matrix.Scale(scale_vector);
	//calculate local rotation
	local_rotate_matrix.RotationY(y_rotation_rads);
	//calculate local transform matrix
	local_transform = local_scale_matrix * local_rotate_matrix;
	
	

}