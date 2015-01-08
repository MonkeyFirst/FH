#include "Common.h"
#include "GameMain.h"

DEFINE_APPLICATION_MAIN(GameMain)

GameMain::GameMain(Context* context) : Application(context)
{
	engineParameters_["WindowTitle"] = GetTypeName();
	engineParameters_["FullScreen"]  = false;
	engineParameters_["Headless"]    = false;
	engineParameters_["WindowWidth"] = 1280;
	engineParameters_["WindowHeight"] = 720; 
	engineParameters_["LogName"]     = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
	engineParameters_["RenderPath"] = "Bin\CoreData\RenderPaths\Forward.xml";
	//engineParameters_["RenderPath"] = "Bin\CoreData\RenderPaths\DeferredSAO.xml";
	//engineParameters_["FlushGPU"] = true;
	RegistrateScripts();

}

void GameMain::RegistrateScripts() 
{
	//Register my scripts
	Character::RegisterObject(context_);
	ThirdPersonCamera::RegisterObject(context_);
	ScriptBoom::RegisterObject(context_);
	ScriptShipAI::RegisterObject(context_);
	ScriptHitFx::RegisterObject(context_);
	ScriptSmokeFx::RegisterObject(context_);
	ScriptFireFx::RegisterObject(context_);
	BotAI::RegisterObject(context_);
	ScriptTailLine::RegisterObject(context_);
}

void GameMain::Start()
{
	Graphics* graphics = GetSubsystem<Graphics>();
	Renderer* renderer = GetSubsystem<Renderer>();
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	
	renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
	renderer->SetTextureAnisotropy(8);
	renderer->SetShadowMapSize(2048);
	renderer->SetShadowQuality(SHADOWQUALITY_HIGH_24BIT);
	renderer->SetDynamicInstancing(true);
	renderer->SetMaterialQuality(QUALITY_HIGH);

	CreateConsole();
	SubscribeToEvents();
	LoadScene("MainScene.xml");
	LoadPrefabs();
	LoadSfx();
	CameraSetup("cameraNode");
	LoadObjects();
	WaterSetup("waterNode");
	Setup2DResources();

	//LEVEL OBJECTS PROCESS
	SetupR2Bots();
	SetupMusic();
}

void GameMain::LoadScene(Urho3D::String sceneFileName) 
{
	scene_ = new Scene(context_);
	
	File sceneFile(context_, 
					GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/" + sceneFileName,
					FILE_READ);

	scene_->LoadXML(sceneFile);
	
}

void GameMain::LoadPrefabs() 
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	world.prefabs.prefabHitFx1_ = cache->GetResource<XMLFile>("Objects/hitfx1.xml");
	world.prefabs.prefabSmokeFx_ = cache->GetResource<XMLFile>("Objects/SmokeFx.xml");
	world.prefabs.prefabFireFx_ = cache->GetResource<XMLFile>("Objects/prefabFireFx.xml");
	world.prefabs.prefabBoomFx_ = cache->GetResource<XMLFile>("Objects/boom.xml");
	world.prefabs.prefabAutogunBullet_ = cache->GetResource<XMLFile>("Objects/AutogunBullet.xml");

	/*world.prefabs.objectsMap["HIT"] = cache->GetResource<XMLFile>("Objects/hitfx1.xml");
	world.prefabs.objectsMap["SMOKE"] = cache->GetResource<XMLFile>("Objects/SmokeFx.xml");
	world.prefabs.objectsMap["FIRE"] = cache->GetResource<XMLFile>("Objects/prefabFireFx.xml");
	world.prefabs.objectsMap["BOOM"] = cache->GetResource<XMLFile>("Objects/boom.xml");*/
}

void GameMain::LoadSfx()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	
	world.sfx.botRunSound = cache->GetResource<Sound>("Sounds/BotRun.ogg");
	world.sfx.botRunSound->SetLooped(true);

	world.sfx.botFireSound = cache->GetResource<Sound>("Sounds/BotFire.ogg");
	world.sfx.hitSound = cache->GetResource<Sound>("Sounds/Hit.ogg");
}



void GameMain::CameraSetup(Urho3D::String cameraNodeName)
{
	Renderer* renderer = GetSubsystem<Renderer>();

	world.camera.node_ = scene_->GetChild(cameraNodeName, true);
	world.camera.camera_ = world.camera.node_->GetComponent<Camera>();
	world.camera.camera_->SetFarClip(1000.0f);
	world.camera.camera_->SetFov(75.0f);
	// Вид
	world.camera.viewport = SharedPtr<Viewport>(new Viewport(context_, scene_, world.camera.camera_));
	// render path
	
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	world.camera.effectRenderPath = world.camera.viewport->GetRenderPath()->Clone();
	
	world.camera.effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
	world.camera.effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));
	world.camera.effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Tonemap.xml"));

	// Make the bloom mixing parameter more pronounced
	world.camera.effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.5f, 0.3f));
	world.camera.effectRenderPath->SetEnabled("Bloom", true);
	world.camera.effectRenderPath->SetEnabled("FXAA2", true);
	world.camera.effectRenderPath->SetEnabled("TonemapUncharted2", true);
	world.camera.effectRenderPath->SetShaderParameter("TonemapMaxWhite", 2.0f);
	world.camera.effectRenderPath->SetShaderParameter("TonemapExposureBias", 2.2f);

	world.camera.viewport->SetRenderPath(world.camera.effectRenderPath);	
	renderer->SetViewport(0, world.camera.viewport);
}


void GameMain::LoadObjects()
{
	Graphics* graphics = GetSubsystem<Graphics>();
	Renderer* renderer = GetSubsystem<Renderer>();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	world.player.node_ = scene_->GetChild("playerNode", true);
	world.player.character_ = world.player.node_->CreateComponent<Character>();
	world.player.character_->gameworld_ = &world;
	world.camera.TPCNode_ = scene_->GetChild("cam", true);
	world.camera.TPCNode_->CreateComponent<ThirdPersonCamera>();

	//booms
	PODVector<Node*> objects; 
	scene_->GetChildrenWithComponent<StaticModel>(objects, true);
	for (int i=0; i< objects.Size(); ++i)
	{
		Variant name_ = objects[i]->GetVar("type");
		if (name_.GetString() == "boom") 
		{
			ScriptBoom* ns = objects[i]->CreateComponent<ScriptBoom>();
		}
	}

	// botty
	world.flyingBot.node_ = scene_->GetChild("botty", true);
	world.flyingBot.botSplinePath_ = world.flyingBot.node_->GetComponent<SplinePath>();

	Node* aiNode_ = scene_->GetChild("AI", true);
	aiNode_->GetChildren(world.flyingBot.aiWaypoints_);
	for (int i = 0; i < world.flyingBot.aiWaypoints_.Size(); i++) 
	{
		world.flyingBot.botSplinePath_->AddControlPoint(world.flyingBot.aiWaypoints_[i]);
	}

	world.flyingBot.botSplinePath_->AddControlPoint(world.flyingBot.aiWaypoints_[0]);
	world.flyingBot.botSplinePath_->SetSpeed(1.0f);
	world.flyingBot.botSplinePath_->SetControlledNode(world.flyingBot.node_);

	//Ship
	world.flyingShips.shipNode_ = scene_->GetChild("shipNode", true);
	world.flyingShips.shipScript_ = world.flyingShips.shipNode_->CreateComponent<ScriptShipAI>();
	world.flyingShips.shipScript_->SetTarget(world.player.node_);

	world.flyingShips.shipNode2_ = scene_->GetChild("shipNode2", true);
	world.flyingShips.shipScript2_ = world.flyingShips.shipNode2_->CreateComponent<ScriptShipAI>();
	world.flyingShips.shipScript2_->SetTarget(world.flyingBot.node_);
	world.flyingShips.shipScript2_->SetMaxSpeed(5.0f);
		
}

//
// Настройка ботов: Создание и привязка скритов.
//
void GameMain::SetupR2Bots() 
{
	PODVector<Node*> objects; 
	scene_->GetChildrenWithComponent<SplinePath>(objects, true);
	//scene_->GetChildrenWithComponent<ScriptInstance>(objects, true);
	
	for (int i=0; i< objects.Size(); ++i)
	{
		Variant name_ = objects[i]->GetVar("tag");
		if (name_.GetString() == "R2BOT") 
		{

			world.R2Bots.botsNodes.Push(objects[i]);
			//ScriptR2Bot* botScript = objects[i]->CreateComponent<ScriptR2Bot>();
			BotAI* botScript = objects[i]->CreateComponent<BotAI>();
			botScript->SetGameWorld(world);
			world.R2Bots.botsLogic.Push(botScript);

		}
	}	
}

void GameMain::Setup2DResources() 
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	UIElement* root = ui->GetRoot();
	//GetSubsystem<Input>()->SetMouseVisible(true);
	root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
	
	Button* b = new Button(context_);
	root->AddChild(b);

	b->SetStyle("Button");
	b->SetSize(300,100);
	b->SetPosition(10,10);

	t = new Text(context_);
	b->AddChild(t);
	t->SetStyle("Text");
	t->SetHorizontalAlignment(HA_CENTER);
	t->SetVerticalAlignment(VA_CENTER);
	t->SetName("Text");
	t->SetVisible(true);	
	t->SetText("Button123");
	
}

void GameMain::SetupMusic()
{
	Audio* audio = GetSubsystem<Audio>();
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	
	music_ = cache->GetResource<Sound>("Music/Lost-Island.ogg");
	if (music_)
	{
		musicNode_ = scene_->CreateChild("musicNode", LOCAL);
		musicSource_ = musicNode_->CreateComponent<SoundSource>();
		music_->SetLooped(true);
		musicSource_->SetSoundType(SOUND_MUSIC);
		//musicSource_->Play(music_);
		GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, 0.05f);
		
		
		
	}

}

void GameMain::WaterSetup(Urho3D::String waterNodeName) 
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Graphics* graphics = GetSubsystem<Graphics>();

	Node* waterNode_ = scene_->GetChild("waterNode",true);
	StaticModel* waterMesh = waterNode_->GetComponent<StaticModel>();
	waterMesh->SetViewMask(0x80000000);
	world.camera.waterPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), waterNode_->GetWorldPosition()); 
	world.camera.waterClipPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), waterNode_->GetWorldPosition() - Vector3(0.0f, 0.1f, 0.0f));
	world.camera.reflectionCameraNode_ = world.camera.node_->CreateChild();
	Camera* reflectionCamera = world.camera.reflectionCameraNode_->CreateComponent<Camera>();
	
	reflectionCamera->SetFarClip(1000.0);
	reflectionCamera->SetFov(75.0f);
	

	reflectionCamera->SetViewMask(0x7fffffff); // Hide objects with only bit 31 in the viewmask (the water plane)
	reflectionCamera->SetAutoAspectRatio(false);
	reflectionCamera->SetUseReflection(true);
	reflectionCamera->SetReflectionPlane(world.camera.waterPlane_);
	reflectionCamera->SetUseClipping(true); // Enable clipping of geometry behind water plane
	reflectionCamera->SetClipPlane(world.camera.waterClipPlane_);
	reflectionCamera->SetAspectRatio((float)graphics->GetWidth() / (float)graphics->GetHeight());

	int texSize = 1024;
	SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
	renderTexture->SetSize(texSize, texSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture->SetFilterMode(FILTER_BILINEAR);
	RenderSurface* surface = renderTexture->GetRenderSurface();
	SharedPtr<Viewport> rttViewport(new Viewport(context_, scene_, reflectionCamera));
	surface->SetViewport(0, rttViewport);
	Material* waterMat = cache->GetResource<Material>("Materials/Water.xml");
	waterMat->SetTexture(TU_DIFFUSE, renderTexture);

}

void GameMain::CreateConsole()
{
	// Get default style
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	// Create console
	Console* console = engine_->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);
	//console->SetVisible(true);

	DebugHud* debugHud = engine_->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
	
}

void GameMain::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, HANDLER(GameMain, HandleUpdate));
	SubscribeToEvent(E_KEYDOWN, HANDLER(GameMain, HandleKeyDown));
	SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(GameMain, HandleMouseDown));

	SubscribeToEvent(E_SCENEUPDATE, HANDLER(GameMain, HandleSceneUpdate));
	//SubscribeToEvent(E_RENDERUPDATE, HANDLER(GameMain, HandleRenderUpdate));

	//UI
	//SubscribeToEvent(E_UIMOUSECLICK, HANDLER(GameMain, HandleControlClicked));


}

void GameMain::HandleYouMostResetThis(StringHash eventType, VariantMap& eventData)
{
	// we get this event every time then user press the button.
	world.flyingBot.botSplinePath_->Reset();	
}


void GameMain::UpdateBotPosition(float timeStep) 
{
	if (world.flyingBot.botSplinePath_->IsFinished()) world.flyingBot.botSplinePath_->Reset();
	world.flyingBot.botSplinePath_->Move(timeStep);

}

void GameMain::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();


	//CameraViewRotate(timeStep);
}

void GameMain::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;
	int key = eventData[P_BUTTON].GetInt();

	/*if (key == MOUSEB_LEFT)
	{
	Vector3 hitPos;
	Vector3 hitNormal;
	Drawable* hitDrawable;
	if (Raycast(250.0f, hitPos, hitNormal, hitDrawable))
	{
	Quaternion q;
	q.FromRotationTo(hitPos, hitNormal);
	Node* hitFx = scene_->InstantiateXML(world.prefabs.prefabHitFx1_->GetRoot(), hitPos, q, LOCAL);
	hitFx->CreateComponent<ScriptHitFx>();
	SharedPtr<SoundSource3D> source = SharedPtr<SoundSource3D>(hitFx->CreateComponent<SoundSource3D>());
	source->Play(world.sfx.hitSound);
	source->SetFarDistance(20.0f);
	source->SetAutoRemove(true);

	}
	}*/

}


void GameMain::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;
	int key = eventData[P_KEY].GetInt();
	
	if (key == KEY_ESC)
	{

		Input* input_ = GetSubsystem<Input>();

		if (input_->IsMouseVisible())
		{
			input_->SetMouseVisible(false);

		}
		else
		{
			input_->SetMouseVisible(true);

			Console* console = GetSubsystem<Console>();
			if (console->IsVisible())
				console->SetVisible(false);
			else
				engine_->Exit();

		}
	}
	else if (key == KEY_F1) 
	{
		Console* console = GetSubsystem<Console>();
		console->Toggle();
	}

	if (key == KEY_F2) drawDebug_ = !drawDebug_;

	if (key == KEY_F3) GetSubsystem<DebugHud>()->ToggleAll();

	if (key == KEY_F) 
	{
		static bool flee = false;
		world.flyingShips.shipScript_->SetFlee(flee);
		flee = !flee;
	}

	if (key == KEY_G) 
	{

	}



}

void GameMain::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();
	
	UpdateBotPosition(timeStep);

}

void GameMain::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
#if 0 // debug waypoints trigger

	DebugRenderer* debug = scene_->GetComponent<DebugRenderer>();
	
	if (world.flyingBot.aiWaypoints_.Size() > 1)
	for(int i=0; i < world.flyingBot.aiWaypoints_.Size()-1; ++i) 
	{
		debug->AddLine(world.flyingBot.aiWaypoints_[i]->GetWorldPosition(),world.flyingBot.aiWaypoints_[i+1]->GetWorldPosition(), Color(1.0f, 1.0f, 0.0f));

	}
	debug->AddLine(world.flyingBot.aiWaypoints_[0]->GetWorldPosition(),world.flyingBot.aiWaypoints_[world.flyingBot.aiWaypoints_.Size()-1]->GetWorldPosition(), Color(1.0f, 1.0f, 0.0f));


	if (world.flyingBot.botSplinePath_ != NULL)
		world.flyingBot.botSplinePath_->DrawDebugGeometry(debug, true);

#endif
}

bool GameMain::Raycast(float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable)
{
	hitDrawable = 0;

	Graphics* graphics = GetSubsystem<Graphics>();
	//Ray cameraRay = camera->GetScreenRay((float)graphics->GetWidth() * 0.5f, (float) graphics->GetHeight() * 0.5f);
	Ray ray = world.camera.camera_->GetScreenRay((float) 0.5f, (float) 0.5f);
	//Ray gunRay = Ray(gunNode_->GetWorldPosition(), gunNode_->GetWorldDirection());

	// Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
	PODVector<RayQueryResult> results;
	//RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	RayOctreeQuery query(results, ray, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY, 191);
		
	Octree* octree = scene_->GetComponent<Octree>();
	//octree->RaycastSingle(query);
	octree->Raycast(query);

	if (results.Size())
	{
		for (int i = 0; i < results.Size(); i++) 
		{
			RayQueryResult& result = results[i];

			Variant fx = result.node_->GetVar("fx");
			if (fx.GetInt() == 1) continue;

			hitPos = result.position_;
			hitNormal = result.normal_;
			hitDrawable = result.drawable_;

			return true;
		}			
	}

	return false;
}