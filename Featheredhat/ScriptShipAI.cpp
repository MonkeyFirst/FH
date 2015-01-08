#include "Common.h"
#include "ScriptShipAI.h"


ScriptShipAI::ScriptShipAI(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE);

}

void ScriptShipAI::RegisterObject(Context* context)
{
	context->RegisterFactory<ScriptShipAI>();
}

void ScriptShipAI::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>(); 
	pos = GetNode()->GetWorldPosition();
	vel = Vector3::ZERO;
	max_speed = 2.0f;
	max_force = 1.0f;
	mass = 10.0f;
	flee_ = false;
	slowingRadius_ = 15;

	rigidbody_ = GetNode()->GetComponent<RigidBody>();
}

void ScriptShipAI::Update(float timeStep) 
{

}

void ScriptShipAI::FixedUpdate(float timeStep)
{
	if (targetNode_ == NULL) return;
	pos = GetNode()->GetWorldPosition();
	Vector3 targetPos = targetNode_->GetWorldPosition();
	Vector3 desired_velocity;

	desired_velocity = (targetPos - pos).Normalized() * max_speed;
	float distance_ = (targetPos - pos).Length();
	
	if (distance_ < slowingRadius_) // slowdown moves 
	{
		desired_velocity = (targetPos - pos).Normalized() * max_speed* (distance_ / slowingRadius_);
	}
	else // default speed;
	{
		desired_velocity = (targetPos - pos).Normalized() * max_speed;
	}
	
	if (flee_)
		desired_velocity = -desired_velocity;

	Vector3 steering = desired_velocity - vel;
	steering = steering.Normalized() * max_force;
	steering = steering / mass;
	vel = vel+steering;
	
	
	//pos += vel * timeStep;
	GetNode()->LookAt(Vector3(targetPos.x_, pos.y_, targetPos.z_));
	rigidbody_->SetLinearVelocity(vel);
}

void ScriptShipAI::SetMaxSpeed(float speed) 
{
	max_speed = speed;
}

void ScriptShipAI::SetTarget(Node* target_) 
{
	if (target_)
		targetNode_ = target_;
}

void ScriptShipAI::Truncate(Vector3& v, float max) 
{
	float f = max / v.Length();
	f < 1.0f ? f : 1.0f; 
	v = v * f;
}

void ScriptShipAI::SetFlee(bool flee) 
{
	flee_ = flee;
}