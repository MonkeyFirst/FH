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

#include "BigBotStates.h"
#include "GameMain.h"
#include "Gameworld.h"


class BigBot : public LogicComponent
{
	OBJECT(BigBot);
public:
	friend class BigBotAIState; // стейты друзья 
	friend class BigBotAIStateIdle;
	friend class BigBotAIStateIdleLookLeft;
	friend class BigBotAIStateIdleLookRight;
	friend class BigBotAIStateIdleTurnLeft;
	friend class BigBotAIStateIdleTurnRight;
	friend class BigBotAIStateAttackWithGun;
	friend class BigBotAIStateHited;
	friend class BigBotAIStateHitedIdle;
	friend class BigBotAIStateAttackWithClaw;
	friend class BigBotAIStateHitedUp;
	friend class BigBotAIStateSkill1A;
	friend class BigBotAIStateSkill1B;

	const float SPEED = 3.0f;
	const float ATTACKRANGE = 20.0f;
	const float ATTACKRANGE_LOW = 8;


	GameWorld* gameWorld_;

	BigBot(Context* context);
	static void RegisterObject(Context* context);
	virtual void Start();
	virtual void Stop();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);

	//bool Raycast(Ray& ray, float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable);

	void SetGameWorld(struct GameWorld& gameWorld);

	void HandleAnimationTrigger(StringHash eventType, VariantMap& eventData);
	void HandleNewIdleState(StringHash eventType, VariantMap& eventData);
	void HandleBotDestroy(StringHash eventType, VariantMap& eventData);
	void HandleCheckPoint(StringHash eventType, VariantMap& eventData);
	void HandleHit(StringHash eventType, VariantMap& eventData);

	BigBotAIState* state_;
	BigBotAIState::ANIMStates animState_;
	BigBotAIState::ANIMStates animPrevState_;

	void ChangeState(BigBotAIState* newState);
	void ClearPrevAnimStates();
	void CheckForFireByPlayer(float range_);

	bool isAttacking;
	Node* target;
	
	BigBotAIState* states[BigBotAIState::ANI_LAST];


protected:
	SharedPtr<Node> node_;
	SharedPtr<AnimatedModel> animModel_;
	SharedPtr<AnimationController> animController_;
	SharedPtr<AnimationState> animIdle_;
	SharedPtr<AnimationState> animTurnLeft_;
	SharedPtr<AnimationState> animTurnRight_;
	SharedPtr<AnimationState> animLookLeft_;
	SharedPtr<AnimationState> animLookRight_;
	SharedPtr<AnimationState> animWalk_;
	SharedPtr<AnimationState> animSkill1A_;
	SharedPtr<AnimationState> animSkill1B_;
	SharedPtr<AnimationState> animAttackWithGun_;
	SharedPtr<AnimationState> animHited_;
	SharedPtr<AnimationState> animHitedIdle_;
	SharedPtr<AnimationState> animAttackWithClaw_;
	SharedPtr<AnimationState> animHitedUp_;


	WeakPtr<RigidBody> rigidbody_;
	SharedPtr<Node> footL_;
	SharedPtr<Node> footR_;

	Bone* boneCenter_;

	SharedPtr<Node> helperTruba1_;
	SharedPtr<Node> helperTruba2_;
	SharedPtr<Node> helperTruba3_;

	SharedPtr<Node> helperSmoke[5];
	
	SharedPtr<Node> laserJoint;
	SharedPtr<AnimatedModel> laserModel;
	SharedPtr<AnimationState> laserAnimState;
	SharedPtr<Node> laserNode;

	
	void SmokeAnimation(float timeStep);
	int cur_stage;
	int stage;
	float stage_time;



};