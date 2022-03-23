#include "Guard.h"
#include "MazeScene.h"
#include "Wall.h"
#include "Waypoint.h"
#include "raylib.h"
#include "Transform2D.h"
#include "PathfindComponent.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include <iostream>

Guard::Guard(float x, float y, float maxSpeed, float maxForce, int color, Maze* maze)
	: Agent(x, y, "Guard", maxSpeed, maxForce)
{
	m_maze = maze;
	getTransform()->setScale({ Maze::TILE_SIZE,Maze::TILE_SIZE });
	m_pathfindComponent = new PathfindComponent(maze);
	m_pathfindComponent->setColor(color);
	addComponent(m_pathfindComponent);
	addComponent(new SpriteComponent("Images/enemy.png"));
}

Guard::~Guard()
{
	delete m_pathfindComponent;
}

void Guard::update(float deltaTime)
{
	m_waypoints = m_maze->getWaypoints();
	Agent::update(deltaTime);
}

void Guard::draw()
{
	Agent::draw();
}

void Guard::onCollision(Actor* other)
{
	if (Wall* wall = dynamic_cast<Wall*>(other)) {
		MathLibrary::Vector2 halfTile = { Maze::TILE_SIZE / 2.0f, Maze::TILE_SIZE / 2.0f };
		MathLibrary::Vector2 position = getTransform()->getWorldPosition();
		position = position + halfTile;
		MathLibrary::Vector2 tilePosition = {
			roundf(position.x / Maze::TILE_SIZE) * Maze::TILE_SIZE,
			roundf(position.y / Maze::TILE_SIZE) * Maze::TILE_SIZE
		};
		tilePosition = tilePosition - halfTile;
		getTransform()->setWorldPostion(tilePosition);

		getMoveComponent()->setVelocity({ 0, 0 });
	}
	if (dynamic_cast<Waypoint*>(other)) 
	{
		std::cout << "I collided" << std::endl;
		Guard::changePatrol();
	}
}

void Guard::setTarget(Actor* target)
{
	m_target = target;
	m_pathfindComponent->setTarget(target);
}

void Guard::changePatrol()
{
	DynamicArray<Actor*> newWaypoints = m_waypoints;
	newWaypoints.remove(m_target);
	int point = rand() % newWaypoints.getLength();
	m_target = m_waypoints[point];
}

Actor* Guard::getTarget()
{
	return m_target;
}
