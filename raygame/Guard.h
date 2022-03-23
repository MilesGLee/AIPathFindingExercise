#pragma once
#include "Agent.h"

class Maze;
class PathfindComponent;

class Guard : public Agent
{
public:
	enum class State
	{
		PATROL,
		CHASE
	};

	Guard(float x, float y, float speed, float maxForce, int color, Maze* maze, Actor* player);
	~Guard();

	virtual void update(float deltaTime) override;
	virtual void draw() override;

	virtual void onCollision(Actor* other) override;

	/// <returns>The current target</returns>
	Actor* getTarget();
	/// <summary>
	/// Set the target of the ghost
	/// </summary>
	/// <param name="target">The new target</param>
	void setTarget(Actor* target);

	int getDistance(Actor* one, Actor* two);
	void changePatrol();

private:
	PathfindComponent* m_pathfindComponent;
	Maze* m_maze;

	Actor* m_target = nullptr;
	Actor* m_player;
	DynamicArray<Actor*> m_waypoints;
	State m_state;
};

