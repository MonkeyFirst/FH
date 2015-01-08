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
//#include <Urho3D/Graphics/DrawableEvents.h>
//#include <Urho3D/Physics/RigidBody.h>

class ScriptShipAI : public LogicComponent 
{
	OBJECT(ScriptShipAI);
public:
	/// Construct.
	ScriptShipAI(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);
	void SetTarget(Node* target_);
	void Truncate(Vector3& v, float max);
	void SetFlee(bool flee);
	void SetMaxSpeed(float speed);
	
	
private:
	Vector3 pos;
	Vector3 vel;
	float max_speed;
	float max_force;
	float mass;
	bool flee_;
	float slowingRadius_;

	WeakPtr<Node> targetNode_;
	WeakPtr<RigidBody> rigidbody_;
	

};