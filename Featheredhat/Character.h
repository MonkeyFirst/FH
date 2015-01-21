#pragma once
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Scene/AnimationDefs.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/ValueAnimation.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Scene/SplinePath.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundListener.h>

#include "GameMain.h"

//namespace Urho3D 
//{
//	class AnimatedModel;
//	class AnimationState;
//	class AnimationController;
//	class Node;
//	class RigidBody;
//	class SoundListener;
//	
//	
//}

using namespace Urho3D;

class Character;

class CharacterState
{
protected:
	Character* character_;

public:
	enum CharacterAnimationStates { ANI_NONE = 0, ANI_FLYING_IDLE, ANI_FLYING_FORWARD, ANI_FIRE_AUTO_GUN };
	enum CharacterTypeStates { STATE_NONE = 0, STATE_FLYING_IDLE, STATE_FLYING_FORWARD, STATE_FIRE_AUTO_GUN, STATE_FIRE, STATE_FALLING };

	CharacterTypeStates stateType_;
	CharacterAnimationStates stateAnimation_;

	float playedTime_;

	CharacterState() {};
	CharacterState(Character* character) { SetActor(character); };
	void SetActor(Character* character) { character_ = character; };
	virtual void EnterState();
	virtual void ExitState();
	virtual void Update(float timeStep) {};
	virtual void FixedUpdate(float timeStep) {};
	virtual ~CharacterState() { character_ = NULL; };



};

class CharacterFlyingIdle : public CharacterState
{
public:
	CharacterFlyingIdle(){};
	CharacterFlyingIdle(Character* character)
	{
		stateAnimation_ = CharacterAnimationStates::ANI_FLYING_IDLE;
		stateType_ = CharacterTypeStates::STATE_FLYING_IDLE;

		SetActor(character);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual ~CharacterFlyingIdle() {};
};

class CharacterFlyingForward : public CharacterState
{
public:
	CharacterFlyingForward(){};
	CharacterFlyingForward(Character* character)
	{
		stateAnimation_ = CharacterAnimationStates::ANI_FLYING_FORWARD;
		stateType_ = CharacterTypeStates::STATE_FLYING_FORWARD;

		SetActor(character);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual ~CharacterFlyingForward() {};
};

class CharacterFireWithAutoGun : public CharacterState
{
public:
	CharacterFireWithAutoGun(){};
	CharacterFireWithAutoGun(Character* character)
	{
		stateAnimation_ = CharacterAnimationStates::ANI_FIRE_AUTO_GUN;
		stateType_ = CharacterTypeStates::STATE_FIRE_AUTO_GUN;

		SetActor(character);
	};
	virtual void EnterState();
	virtual void ExitState();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual ~CharacterFireWithAutoGun() {};
};


class Character : public LogicComponent 
{
	OBJECT(Character);
public:
	friend class CharacterState;
	friend class CharacterFlyingIdle;
	friend class CharacterFlyingForward;
	friend class CharacterFireWithAutoGun;

	struct GameWorld* gameworld_;

	/// Construct.
	Character(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);

	virtual void FixedUpdate(float timeStep);

	CharacterState* state_;
	CharacterState::CharacterAnimationStates animState_;
	CharacterState::CharacterAnimationStates animPrevState_;

	void ChangeState(CharacterState* newState);
	void ClearPrevAnimStates();
	bool Raycast(float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable);
	bool RaycastAnyByTag(String tag, float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable);
	bool PhysicRaycastRigidBodyByNodeTag(String tag, float maxDistance, Vector3& hitPos, Vector3& hitNormal, Node*& hitNode);

private:
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
	void HandleGlobalBlastEvent(StringHash eventType, VariantMap& eventData);
	void HandleAnimationTrigger(StringHash eventType, VariantMap& eventData);


	SharedPtr<RigidBody> rigidbody_;
	Quaternion rotation_;
	SharedPtr<Node> cameraNode_;
	SharedPtr<Node> nodeBird_;
	SharedPtr<Node> nodeGirl_;
	SharedPtr<Node> nodeGuns_;

	SharedPtr<AnimatedModel> modelBird_;
	SharedPtr<AnimatedModel> modelGirl_;
	SharedPtr<AnimatedModel> modelGuns_;

	SharedPtr<AnimationState> animStateBirdIdle_;
	SharedPtr<AnimationState> animStateGirlIdle_;
	SharedPtr<AnimationState> animStateGunsIdle_;

	SharedPtr<AnimationState> animStateBirdForward_;
	SharedPtr<AnimationState> animStateGirlForward_;

	SharedPtr<AnimationState> animStateBirdFire_;
	SharedPtr<AnimationState> animStateGirlFire_;
	SharedPtr<AnimationState> animStateGunsFire_;

	Bone* FireFxJoint;
	SharedPtr<Node> AutogunBulletJoint_;
	SharedPtr<Light> AutogunLight_;

	SharedPtr<SoundListener> listener_;
	CharacterState* states[9];

};