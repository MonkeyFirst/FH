#include "Common.h"
#include "LifeTime.h"

/// Construct.
LifeTime::LifeTime(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE);
}

void LifeTime::SetLifeTime(float lifeTime) 
{
	maxLifeTime_ = lifeTime;
}

/// Register object factory and attributes.
void LifeTime::RegisterObject(Context* context) 
{
	context->RegisterFactory<LifeTime>();
}

void LifeTime::Start() 
{
	currentLifeTime_ = 0.0f;
}

void LifeTime::Update(float timeStep) 
{

}

void LifeTime::FixedUpdate(float timeStep)
{
	currentLifeTime_ += timeStep;
	if (currentLifeTime_ > maxLifeTime_)
	{
		
		GetNode()->Remove();
	}
}