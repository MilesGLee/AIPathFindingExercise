#include "Guard.h"
#include "MazeScene.h"
#include "Wall.h"
#include "Waypoint.h"
#include "raylib.h"
#include "Transform2D.h"
#include "PathfindComponent.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "AABBCollider.h"
#include "Transform2D.h"
#include <iostream>

Guard::Guard(float x, float y, float maxSpeed, float maxForce, int color, Maze* maze, Actor* player)
	: Agent(x, y, "Guard", maxSpeed, maxForce)
{
	m_maze = maze;
	getTransform()->setScale({ Maze::TILE_SIZE,Maze::TILE_SIZE });
	setCollider(new AABBCollider(Maze::TILE_SIZE, Maze::TILE_SIZE, this));
	m_pathfindComponent = new PathfindComponent(maze);
	m_pathfindComponent->setColor(color);
	addComponent(m_pathfindComponent);
	addComponent(new SpriteComponent("Images/enemy.png"));
	m_state = State::PATROL;
	m_player = player;
}

Guard::~Guard()
{
	delete m_pathfindComponent;
}

void Guard::update(float deltaTime)
{
	//std::cout << getDistance(m_player, this) << std::endl;

	m_waypoints = m_maze->getWaypoints(); //Update the current waypoints the map has
	if (getDistance(m_player, this) < 150 && m_state == State::PATROL) //If the player is in range and this is patrolling, start chasing the player
	{
		m_state = State::CHASE;
		setTarget(m_player);
	}
	if (getDistance(m_player, this) > 250 && m_state == State::CHASE) //If this is chasing the player and the player is outside of the range, go back to patrolling
	{
		m_state = State::PATROL;
		changePatrol();
	}
		
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
	if (dynamic_cast<Waypoint*>(other) && other == m_target) //If the enemy collides with its target waypoint, pick a new random one.
	{
		if(m_state == State::PATROL)
			changePatrol();
	}
}

void Guard::setTarget(Actor* target)
{
	m_target = target;
	m_pathfindComponent->setTarget(target);
}

int Guard::getDistance(Actor* one, Actor* two) //Gets the distance between two actors positions
{
	return sqrt(((one->getTransform()->getWorldPosition().x - two->getTransform()->getWorldPosition().x) * (one->getTransform()->getWorldPosition().x - two->getTransform()->getWorldPosition().x)) + ((one->getTransform()->getWorldPosition().y - two->getTransform()->getWorldPosition().y) * (one->getTransform()->getWorldPosition().y - two->getTransform()->getWorldPosition().y)));
}

void Guard::changePatrol() //Picks a random waypoint excluding the already targetted one.
{
	DynamicArray<Actor*> newWaypoints = m_waypoints;
	newWaypoints.remove(m_target);
	int point = rand() % newWaypoints.getLength();
	setTarget(newWaypoints[point]);
}

Actor* Guard::getTarget()
{
	return m_target;
}
