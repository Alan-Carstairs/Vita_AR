#pragma once
#include "GameObject.h"
#include <vector>

class Projectiles : public GameObject
{
public:

	void Init(gef::Vector4 enemy_pos);

	void Update(float frame_time);
	void Cal_Velocity(gef::Vector4, gef::Vector4);
	float p_speed_;
	//float DisToEnemy;

	std::vector<gef::Vector4> Projectile_Pos;

};