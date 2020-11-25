#include "Projectiles.h"

void Projectiles::Init(gef::Vector4 enemy_pos)
{
	p_speed_ = 0.21f;
	scale_vector = gef::Vector4(2, 2, 2);
	position_ = gef::Vector4(0, 0, 0);
	velocity_ = enemy_pos - position_;
	velocity_.Normalise();
	velocity_ *= p_speed_;
}

void Projectiles::Update(float frame_time)
{
 	position_ += velocity_ * frame_time;
	BuildTransform();
}

void Projectiles::Cal_Velocity(gef::Vector4 enemy_pos, gef::Vector4 tower_pos)
{
	velocity_ = enemy_pos - tower_pos;
	velocity_.Normalise();
	velocity_ *= p_speed_;
}
