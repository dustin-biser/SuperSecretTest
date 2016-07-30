#include "C_Application.h"

#include <cstdlib>
using std::rand;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include "graphics.h"
#include "time.h"

#include "Core/GameConstants.hpp"

#include "Assets/AssetDefinitions.hpp"
#include "Assets/AssetLoader.hpp"
#include "Assets/Mesh2d.hpp"

#include "Core/GameObject.hpp"

#include "Rendering/Rendering.hpp"



// TODO - Remove after testing...
#include "Core/ComponentPool.hpp"
#include "Core/ComponentPoolLocator.hpp"



static vec2 randomPositionBetween (vec2 min, vec2 max); 



class C_ApplicationImpl {
private:
	friend class C_Application;

	const int m_ScreenWidth;
	const int m_ScreenHeight;

	// TODO - Move these into Cannon GameObject
	int	m_CannonX;
	int	m_CannonY;

	std::unordered_map<MeshId, Mesh2d> meshAssetDirectory;

	GameObject * cannon;
	GameObject * projectilePrototype;
	GameObject * clockPrototype;

#if false
	// Allocation Pools
	ComponentPool * gameObjectPool;
	ComponentPool * childGameObjectPool;
	ComponentPool * prototypePool;

	GameObjectID cannon_id;

	GameObjectReplicator * clockReplicator;
	GameObjectReplicator * projectileReplicator;


	// Subsystems
	RenderingSystem * graphicsSystem;
	MotionSystem * motionSystem;
#endif

	C_ApplicationImpl (
		int screenWidth,
		int screenHeight
	);

	void buildMeshAssetDirectory();

	void loadGameObjects();

	void Tick (
		C_Application::T_PressedKey pressedKeys
	);

	void handleInput (
		C_Application::T_PressedKey pressedKeys
	);

};

//---------------------------------------------------------------------------------------
C_ApplicationImpl::C_ApplicationImpl(
	int screenWidth,
	int screenHeight
) 
	: m_ScreenWidth(screenWidth),
	  m_ScreenHeight(screenHeight),
	  m_CannonX(m_ScreenWidth / 2),
	  m_CannonY(m_ScreenHeight / 2)
{
	buildMeshAssetDirectory();

	loadGameObjects();
}

//---------------------------------------------------------------------------------------
void C_ApplicationImpl::buildMeshAssetDirectory()
{
	Asset::Definition assetDefinitions[] = {
		Asset::Cannon,
		Asset::Projectile,
		Asset::ClockBase,
		Asset::HourHand,
		Asset::MinuteHand,
		Asset::SecondHand
	};

	for (auto asset : assetDefinitions) {
		Mesh2d mesh;
		AssetLoader::decodeMesh(asset, mesh);
		meshAssetDirectory[mesh.meshId] = std::move(mesh);
	}
}


//---------------------------------------------------------------------------------------
//void C_ApplicationImpl::initGameObjectReplicators()
//{
#if false

	// Initialize clock replicator
	{
		Rendering * hourHandGraphics = new Rendering(
			Color{ 1.0f, 1.0f, 1.0f },
			&meshAssetDirectory.at("HourHand")
		);

		Rendering * minuteHandGraphics = new Rendering(
			Color{ 1.0f, 0.3f, 0.3f },
			&meshAssetDirectory.at("MinuteHand")
		);

		Rendering * secondHandGraphics = new Rendering(
			Color{ 0.8f, 0.8f, 0.2f },
			&meshAssetDirectory.at("SecondHand")
		);

		Rendering * clockBaseGraphics = new Rendering(
			Color{ 0.8f, 0.2f, 0.2f },
			&meshAssetDirectory.at("ClockBase")
		);

		Motion * clockHandMotion = new ClockHandMotionComponent();

		GameObject * hourHand = childGameObjectPool->create(GameObject::generateID());
		hourHand->graphics = hourHandGraphics;
		hourHand->motion = clockHandMotion;

		GameObject * minuteHand = childGameObjectPool->create(GameObject::generateID());
		minuteHand->graphics = minuteHandGraphics;
		minuteHand->motion = clockHandMotion;

		GameObject * secondHand = childGameObjectPool->create(GameObject::generateID());
		secondHand->graphics = secondHandGraphics;
		secondHand->motion = clockHandMotion;


		GameObject * clockPrototype = prototypePool->create(GameObject::generateID());

		clockPrototype->addChild(hourHand);
		clockPrototype->addChild(minuteHand);
		clockPrototype->addChild(secondHand);

		clockPrototype->graphics = clockBaseGraphics;

		float scale_x = (100.0f / m_ScreenWidth);
		float scale_y = (100.0f / m_ScreenHeight);
		clockPrototype->transform.scale = vec2(scale_x, scale_y);

		clockReplicator = new GameObjectReplicator(clockPrototype);
	}

	// Initialize projectile replicator
	{
		Rendering * projectileGraphicsComponent = new Rendering (
			Color{ 1.0f, 1.0f, 1.0f },
			&meshAssetDirectory.at("Projectile")
		);

		GameObject * projectilePrototype = prototypePool->create(GameObject::generateID());
		projectilePrototype->graphics = projectileGraphicsComponent;
		projectilePrototype->motion->velocity = vec2(0.0f, 1.5f);

		float scale_x = (30.0f / m_ScreenWidth);
		float scale_y = (30.0f / m_ScreenHeight);
		projectilePrototype->transform.scale = vec2(scale_x, scale_y);
		projectilePrototype->transform.position = vec2(-0.8f, 0.0f);

		projectileReplicator = new GameObjectReplicator(projectilePrototype);
	}
#endif
//}


//---------------------------------------------------------------------------------------
static vec2 randomPositionBetween (
	vec2 min,
	vec2 max
) {
	float t0 = rand() / static_cast<float>(RAND_MAX);
	float t1 = rand() / static_cast<float>(RAND_MAX);

	float x = (1.0f - t0) * min.x + t0 * max.x;
	float y = (1.0f - t1) * min.y + t1 * max.y;

	return vec2(x, y);
}

//---------------------------------------------------------------------------------------
void C_ApplicationImpl::loadGameObjects()
{
	cannon = new GameObject("Cannon");
	Rendering * renderingComponent = cannon->addComponent<Rendering>();
	renderingComponent->mesh = &meshAssetDirectory.at("Cannon");
	renderingComponent->color = Color { 0.2f, 0.2f, 1.0f };

	float scale_x = (60.0f / m_ScreenWidth);
	float scale_y = (60.0f / m_ScreenHeight);
	cannon->transform()->scale = vec2(scale_x, scale_y);

	// Place cannon near bottom of screen.
	cannon->transform()->position = vec2(0.0f, -0.8f);


	// TODO - Test creating + destroying components
	{
		GameObject * gameObject[30];
		for (int i (0); i < 10; ++i) {
			int index;
			for (int j(0); j < 3; ++j) {
				index = 3*i + j;
				char buffer[32];
				sprintf_s(buffer, "GameObject %d", index);
				gameObject[index] = new GameObject(buffer);
				gameObject[index]->transform ()->position = vec2 (float(i));
			}
			gameObject[index]->setActive(false);
		}
	}

#if false
	// Load Cannon
	{
		// Keep track of cannon for later use.
		this->cannon_id = GameObject::generateID();

		GameObject * cannon = gameObjectPool->createComponent(cannon_id);
		cannon->graphics = new Rendering (
			Color{ 0.2f, 0.2f, 1.0f },
			&meshAssetDirectory.at("Cannon")
		);
		float scale_x = (60.0f / m_ScreenWidth);
		float scale_y = (60.0f / m_ScreenHeight);
		cannon->transform.scale = vec2(scale_x, scale_y);

		// Place cannon near bottom of screen.
		cannon->transform.position = vec2(0.0f, -0.8f);
	}

	// Load 2 Clocks in random positions
	{
		GameObject * clock1 = clockReplicator->replicateTo(gameObjectPool);
		clock1->transform.position = randomPositionBetween(vec2(-0.7f, 0.0f), vec2(-0.2f, 0.8f));
		clock1->motion->velocity = randomPositionBetween(vec2(-0.05f, -0.05f), vec2(0.05f, 0.05f));


		GameObject * clock2 = clockReplicator->replicateTo(gameObjectPool);
		clock2->transform.position = randomPositionBetween(vec2(0.2f, 0.0f), vec2(0.7f, 0.8f));
		clock2->motion->velocity = randomPositionBetween(vec2(-0.05f, -0.05f), vec2(0.05f, 0.05f));
	}
#endif
}

//---------------------------------------------------------------------------------------
void C_ApplicationImpl::Tick (
	C_Application::T_PressedKey pressedKeys
) {

#if false
	handleInput(pressedKeys);

	float ellapsedTimeInSconds = 1.0f / 50.0f;
	motionSystem->update(gameObjectPool, ellapsedTimeInSconds);

	graphicsSystem->clearScreen(m_ScreenWidth, m_ScreenHeight);
	graphicsSystem->drawGameObjects(gameObjectPool->beginActive(), gameObjectPool->numActive());
#endif
}

//---------------------------------------------------------------------------------------
void C_ApplicationImpl::handleInput (
	C_Application::T_PressedKey pressedKeys
) {
#if false
	const float deltaAngle = 0.1f;
	const float maxAngle = k_PI * 0.5f;
	const float minAngle = -maxAngle;

	if (pressedKeys & C_Application::s_KeyLeft) {
		GameObject * cannon = gameObjectPool->getComponent(cannon_id);
		float & rotationAngle = cannon->transform.rotationAngle;
		rotationAngle += deltaAngle;
		rotationAngle = min(maxAngle, rotationAngle);
	}
	
	if (pressedKeys & C_Application::s_KeyRight) {
		GameObject * cannon = gameObjectPool->getComponent(cannon_id);
		float & rotationAngle = cannon->transform.rotationAngle;
		rotationAngle -= deltaAngle;
		rotationAngle = max(minAngle, rotationAngle);
	}

	if (pressedKeys & C_Application::s_KeyUp) {
	}

	if (pressedKeys & C_Application::s_KeyDown) {
	}

	if (pressedKeys & C_Application::s_KeySpace) {
		// Update projectile prototype to spawn at tip of cannon.
		{
			GameObject * projectilePrototype = projectileReplicator->getPrototype();
			GameObject * cannon = gameObjectPool->getComponent(cannon_id);

			// Get vertex at tip of cannon.
			Vertex vertex = cannon->graphics->mesh->vertexList[2];
			Transform cannonTransform = cannon->transform;
			vertex = cannonTransform * vertex;

			// Update projectile's position and orientation
			projectilePrototype->transform.position = vertex;
			projectilePrototype->transform.rotationAngle = cannonTransform.rotationAngle;

			// Update projectile's velocity direction.
			vec2 direction = normalize(vertex - cannon->transform.position);
			vec2 vel = projectilePrototype->motion->velocity;
			projectilePrototype->motion->velocity = direction * length(vel);
		}

		// Generate projectile
		projectileReplicator->replicateTo(gameObjectPool);
	}
#endif
}

//---------------------------------------------------------------------------------------
C_Application::C_Application (
	int screenWidth,
	int screenHeight
) {
	impl = new C_ApplicationImpl(screenWidth, screenHeight);
}


//---------------------------------------------------------------------------------------
C_Application::~C_Application()
{
	delete impl;
	impl = nullptr;
}


//---------------------------------------------------------------------------------------
void C_Application::Tick(T_PressedKey pressedKeys)
{
	impl->Tick(pressedKeys);
}

