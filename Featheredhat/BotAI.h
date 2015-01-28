#pragma once
//#include <Urho3D/Scene/LogicComponent.h>
//#include <Urho3D/Graphics/Geometry.h>
//#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/Animation.h>
//#include <Urho3D/Scene/AnimationDefs.h>
//#include <Urho3D/Graphics/AnimatedModel.h>
//#include <Urho3D/Graphics/AnimationState.h>
//#include <Urho3D/Graphics/AnimationController.h>
//#include <Urho3D/Scene/ValueAnimation.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Scene/SplinePath.h>
//#include <Urho3D/Container/Ptr.h>
//#include <Urho3D/Audio/Sound.h>
//#include <Urho3D/Audio/SoundSource.h>
//#include <Urho3D/Audio/SoundSource3D.h>

#include "Gameworld.h"
#include "GameMain.h"


class BotAI; // early def


///
/// BASE STATE 
/// 
class BotAIState
{
protected:
	BotAI* botAi_;

public:
	enum ANIMStates { ANI_NONE = 0, ANI_IDLE_1, ANI_IDLE_2, ANI_IDLE_3, ANI_IDLE_4, ANI_IDLE_5, ANI_WALK, ANI_FIRE, ANI_FALLING };
	enum AIStates { STATE_NONE = 0, STATE_IDLE, STATE_CHECKPOINT, STATE_FIRE, STATE_FALLING };
	AIStates stateType_;
	ANIMStates stateAni_;

	float playedTime_;

	BotAIState();
	BotAIState(BotAI& bot);
	void SetActor(BotAI* bot);
	virtual void EnterState();
	virtual void ExitState();

	virtual void Update(float timeStep) {};
	virtual void FixedUpdate(float timeStep) {};
	virtual ~BotAIState() { botAi_ = NULL; };



};

///
/// IDLE STATE_1
///
class BotAIStateIdle1 : public BotAIState
{

public:
	BotAIStateIdle1(){};
	BotAIStateIdle1(BotAI* bot)
	{ 
		stateType_ = BotAIState::STATE_IDLE;
		stateAni_ = BotAIState::ANI_IDLE_1;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};

///
/// IDLE STATE_2
///
class BotAIStateIdle2 : public BotAIState
{

public:
	BotAIStateIdle2(){};
	BotAIStateIdle2(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_IDLE;
		stateAni_ = BotAIState::ANI_IDLE_2;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};

///
/// IDLE STATE_3
///
class BotAIStateIdle3 : public BotAIState
{

public:
	BotAIStateIdle3(){};
	BotAIStateIdle3(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_IDLE;
		stateAni_ = BotAIState::ANI_IDLE_3;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};
///
/// IDLE STATE_4
///
class BotAIStateIdle4 : public BotAIState
{

public:
	BotAIStateIdle4(){};
	BotAIStateIdle4(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_IDLE;
		stateAni_ = BotAIState::ANI_IDLE_4;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};
///
/// IDLE STATE_5
///
class BotAIStateIdle5 : public BotAIState
{

public:
	BotAIStateIdle5(){};
	BotAIStateIdle5(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_IDLE;
		stateAni_ = BotAIState::ANI_IDLE_5;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};



///
/// CHECKPOINT STATE
///
class BotAIStateCheckPoint : public BotAIState
{

public:
	BotAIStateCheckPoint(){};
	BotAIStateCheckPoint(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_CHECKPOINT;
		stateAni_ = BotAIState::ANI_WALK;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);

	/// overrided for sfxRun
	void EnterState();
	void ExitState();

	~BotAIStateCheckPoint() {};
};

///
/// FIRE STATE
///
class BotAIStateFire : public BotAIState
{

public:
	BotAIStateFire(){};
	BotAIStateFire(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_FIRE;
		stateAni_ = BotAIState::ANI_FIRE;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);

	/// overrided for boneCenter
	void EnterState();
	void ExitState();


	~BotAIStateFire() {};
};

///
/// FALLING STATE
///
class BotAIStateFalling : public BotAIState
{

public:
	BotAIStateFalling(){};
	BotAIStateFalling(BotAI* bot)
	{
		stateType_ = BotAIState::STATE_FALLING;
		stateAni_ = BotAIState::ANI_FALLING;

		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	~BotAIStateFalling() {};
};





class BotAI : public LogicComponent
{
	OBJECT(BotAI);
public:
	friend class BotAIStateIdle1;
	friend class BotAIStateIdle2;
	friend class BotAIStateIdle3;
	friend class BotAIStateIdle4;
	friend class BotAIStateIdle5;
	friend class BotAIStateCheckPoint;
	friend class BotAIStateFalling;
	friend class BotAIStateFire;
	const float SPEED = 3.0f;
	const float ATTACKRANGE = 8.0f;


	GameWorld* gameWorld_;
	BotAIState* state_;
	BotAIState::ANIMStates animState_;
	BotAIState::ANIMStates animPrevState_;
	bool isRemoved;
	bool isAttacking;

	void ChangeState(BotAIState* newState);
	void ClearPrevAnimStates();
	void ProcessRandomCheckPoint(float timeStep);
	void ProcessEmmitFootSteps(float timeStep);
	Node* GetRandomCheckPoint();
	void CheckForFireByPlayer(float range_);

	BotAI(Context* context);
	static void RegisterObject(Context* context);
	virtual void Start();
	virtual void Stop();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	bool Raycast(Ray& ray, float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable);
	void SetGameWorld(GameWorld& gameWorld);
	
	void HandleAnimationTrigger(StringHash eventType, VariantMap& eventData);
	void HandleAnimationEnds(StringHash eventType, VariantMap& eventData);
	void HandleBotDestroy(StringHash eventType, VariantMap& eventData);
	void HandleCheckPoint(StringHash eventType, VariantMap& eventData);
	void HandleHit(StringHash eventType, VariantMap& eventData);
	void Hit();

	

	void DestroyBot();
	




	//common ai methods
	bool isBotOnGround();
	

protected:
	SharedPtr<Node> node_;
	SharedPtr<AnimatedModel> animModel_;
	SharedPtr<AnimationController> animController_;
	SharedPtr<AnimationState> animIdle1_;
	SharedPtr<AnimationState> animIdle2_;
	SharedPtr<AnimationState> animIdle3_;
	SharedPtr<AnimationState> animIdle4_;
	SharedPtr<AnimationState> animIdle5_;
	SharedPtr<AnimationState> animWalk_;
	SharedPtr<AnimationState> animFalling_;
	SharedPtr<AnimationState> animFire_;
	WeakPtr<RigidBody> rigidbody_;
	PODVector<Node*> checkPoints;
	SharedPtr<Node> aiPoints;
	SharedPtr<Node> fireNode_;
	SharedPtr<Node> rayNode_;
	SharedPtr<Node> footL_;
	SharedPtr<Node> footR_;

	BotAIState* states[9];
	SharedPtr<Node> selectedCheckPoint;
	SharedPtr<Node> target_;
	Bone* boneCenter_;


	SharedPtr<Node> sfxRunNode_;
	SharedPtr<SoundSource3D> sfxRunSource_;

};