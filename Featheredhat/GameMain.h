#pragma once
#include "Common.h"
#include "Gameworld.h"
#include "GameMenu.h"

class GameMain : public Application
{
	OBJECT(GameMain);

public:
	GameMain(Context* context);
	virtual void Start();

protected:
	GameWorld world;
	GameMenu menu;

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
	void SetupBigBots();
	void SetupMusic();
	void CreateMenu();


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
	
	
};