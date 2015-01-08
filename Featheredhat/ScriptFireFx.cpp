#include "Common.h"
#include "ScriptFireFx.h"


ScriptFireFx::ScriptFireFx(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	//SetUpdateEventMask(USE_FIXEDUPDATE);
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);

}

void ScriptFireFx::RegisterObject(Context* context)
{
	context->RegisterFactory<ScriptFireFx>();
}

void ScriptFireFx::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();


	animModel_ = GetNode()->GetComponent<AnimatedModel>();
	animState_ = animModel_->GetAnimationState(StringHash("FIREFX"));
	animState_->SetLayer(0);
	animState_->SetWeight(1.0f);
	animState_->SetTime(0.0f);
	animState_->SetLooped(false);

	sfxFireSource_ = GetNode()->CreateComponent<SoundSource3D>();
	sfxFireSource_->SetFarDistance(10.0f);


}

void ScriptFireFx::Update(float timeStep) 
{
		animState_->AddTime(timeStep);
}

void ScriptFireFx::FixedUpdate(float timeStep)
{

	float t = animState_->GetTime();
	float l = animState_->GetLength(); 
	
	if  ( t >= l ) 
	{
		using namespace AnimationTrigger;
		animState_->SetTime(0.0f);

		VariantMap& eventData = GetNode()->GetEventDataMap();

		eventData[P_DATA] = GetNode()->GetWorldPosition();
		
		SendEvent(StringHash("Blast"), eventData);

		GetNode()->Remove();

	}
}

