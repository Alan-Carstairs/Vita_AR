#pragma once
#include "GameObject.h"
#include <list>
#include <vector>

class Enemy: public GameObject
{
public:
	Enemy();
	void Init();
	bool Update(float frame_time);
	void reCal_Velocity();
	void calculate_Distance(gef::Vector4 s);
	void setup_Waypoints(gef::Vector4 ct, gef::Vector4 nt);
	void init_Waypoints(std::vector<gef::Vector4>);
	
	float speed_;
	int currentWP_, nextWP_;
	float DisToWayPoint;
	float DisBetweenWayPoint;

	std::vector<gef::Vector4> waypoints_Pos;



};