#pragma once
#include "Common.h"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

// My Scripts 
#include "Character.h"
#include "ThirdPersonCamera.h"
#include "ScriptBoom.h"
#include "ScriptShipAI.h"
//#include "ScriptR2Bot.h"
#include "ScriptHitFx.h"
#include "ScriptSmokeFx.h"
#include "ScriptFireFx.h"
#include "BotAI.h"
#include "ScriptTailLine.h"

using namespace Urho3D;

typedef struct GameWorld 
{
	struct  
	{
		SharedPtr<Node> node_;
		WeakPtr<Character> character_;
		

	} player;

	struct 
	{
		SharedPtr<Node> node_;
		SharedPtr<Node> TPCNode_;
		WeakPtr<Camera> camera_;
		SharedPtr<Viewport> viewport;
		SharedPtr<RenderPath> effectRenderPath;

		Plane waterPlane_;
		Plane waterClipPlane_;
		SharedPtr<Node> reflectionCameraNode_;

	} camera;


	struct  
	{
		SharedPtr<Node> node_;
		PODVector<Node*> aiWaypoints_;
		SharedPtr<SplinePath> botSplinePath_;
	} flyingBot;


	struct  
	{
		SharedPtr<Node> shipNode_;
		WeakPtr<ScriptShipAI> shipScript_;
		SharedPtr<Node> shipNode2_;
		WeakPtr<ScriptShipAI> shipScript2_;
	
	} flyingShips;

	struct  
	{
		HashMap<StringHash, SharedPtr<XMLFile>> objectsMap;
		
		SharedPtr<XMLFile> prefabHitFx1_;
		SharedPtr<XMLFile> prefabSmokeFx_;
		SharedPtr<XMLFile> prefabFireFx_;
		SharedPtr<XMLFile> prefabBoomFx_;
		SharedPtr<XMLFile> prefabAutogunBullet_;
		
	} prefabs;


	struct
	{
		SharedPtr<Sound> botRunSound;
		SharedPtr<Sound> botFireSound;
		SharedPtr<Sound> hitSound;


	} sfx;

	struct 
	{
		PODVector<Node*> botsNodes;
		//PODVector<ScriptR2Bot*> botsLogic;
		PODVector<BotAI*> botsLogic;
	} R2Bots;



} GameWorld; 


class GameMain : public Application
{
	OBJECT(GameMain);

public:
	GameMain(Context* context);
	virtual void Start();

protected:
	GameWorld world;
	SharedPtr<Scene> scene_;
	SharedPtr<Sound> music_;
	SharedPtr<Node> musicNode_;
	SharedPtr<SoundSource> musicSource_;


private:
	// SETUP
	void CreateConsole();
	void SubscribeToEvents();
	void RegistrateScripts();

	void LoadScene(Urho3D::String sceneFileName);
	void LoadPrefabs();
	void LoadSfx();
	void CameraSetup(Urho3D::String cameraNodeName);
	void LoadObjects();
	void WaterSetup(Urho3D::String waterNodeName);
	void Setup2DResources();
	void SetupR2Bots();
	void SetupMusic();

	// UPDATE
	void UpdateBotPosition(float timeStep);

	// HANDLES
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleMouseDown(StringHash eventType, VariantMap& eventData);
	void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
	void HandleFixedUpdate();
	void HandleYouMostResetThis(StringHash eventType, VariantMap& eventData);
	void HandleControlClicked(StringHash eventType, VariantMap& eventData);
	bool Raycast(float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable);

	// VARS
	bool paused_;
	bool drawDebug_;

	Text* t; 

};