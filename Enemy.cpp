
#include "Enemy.h"

Enemy::Enemy()
{
	Init();
}

void Enemy::Init()
{
	speed_ = 0.02f;
	currentWP_ = 0;
	nextWP_ = 1;
	scale_vector = gef::Vector4(.002, .002, .002);
}

bool Enemy::Update(float frame_time)
{
	position_ += velocity_* frame_time;
	BuildTransform();
	
	if (DisToWayPoint < DisBetweenWayPoint*0.05f)
	{
		currentWP_++;
		nextWP_++;
		setup_Waypoints(waypoints_Pos[currentWP_], waypoints_Pos[nextWP_]);
		reCal_Velocity();


	}
	if (currentWP_ == 45)
	{
		velocity_ = gef::Vector4(0,0,0,0);
	}
	if (waypoints_Pos.size() > 0)
	{
		calculate_Distance(waypoints_Pos[nextWP_]);
	}
	return true;
}

void Enemy::reCal_Velocity()
{
	velocity_ = waypoints_Pos[nextWP_] - position_;
	velocity_.Normalise();
	velocity_ *= speed_;
}

void Enemy::calculate_Distance(gef::Vector4 start)
{
	DisToWayPoint = (position_ - start).Length();
}

void Enemy::setup_Waypoints(gef::Vector4 st, gef::Vector4 ed)
{
	DisBetweenWayPoint = (ed - st).Length();
}

void Enemy::init_Waypoints(std::vector<gef::Vector4> waypoints)
{
	waypoints_Pos = waypoints;
	calculate_Distance(waypoints_Pos[1]);

}
