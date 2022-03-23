#pragma once
#include "Actor.h"
class Waypoint :
	public Actor
{
public:
	Waypoint(float x, float y);
	void draw() override;

	void onCollision(Actor* actor) override;
};

