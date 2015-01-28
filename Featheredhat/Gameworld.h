#pragma once


class ThirdPersonCamera;
class Character;
class ScriptShipAI;
class BigBot;
class BotAI;

struct GameWorld
{
	struct
	{
		SharedPtr<Node> node_;
		Character* character_;


	} player;

	struct
	{
		SharedPtr<Node> node_;
		SharedPtr<Node> TPCNode_;
		ThirdPersonCamera* TPCLogic_;
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
		ScriptShipAI* shipScript_;
		SharedPtr<Node> shipNode2_;
		ScriptShipAI* shipScript2_;

	} flyingShips;

	struct
	{
		HashMap<StringHash, SharedPtr<XMLFile>> objectsMap;

		SharedPtr<XMLFile> prefabHitFx1_;
		SharedPtr<XMLFile> prefabSmokeFx_;
		SharedPtr<XMLFile> prefabBlackSmokeFx_;
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

	struct
	{
		PODVector<Node*> bigbotsNodes;
		PODVector<BigBot*> bigbotsLogic;
	} Bigbots;



};
