#include "Common.h"
#include "UserScripts.h"


ScriptHitFx::ScriptHitFx(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE);
	//SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);

}

void ScriptHitFx::RegisterObject(Context* context)
{
	context->RegisterFactory<ScriptHitFx>();
}

void ScriptHitFx::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	maxLifeTime = 0.25f; // Секунды
	currentLifeTime_ = 0.0f;

	scaleAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));

	float startSize = 0.5f * GetNode()->GetWorldScale().Length();
	float endSize = 1.0f * GetNode()->GetWorldScale().Length();

	scaleAnim_->SetKeyFrame(0.0f, Vector3(startSize, startSize, startSize));
	scaleAnim_->SetKeyFrame(0.25f, Vector3(endSize, endSize*2, endSize));
	
	GetNode()->SetScale(startSize);
	GetNode()->SetAttributeAnimation("Scale", scaleAnim_, WM_ONCE);

	prefabSmokeFx_ = cache->GetResource<XMLFile>("Objects/SmokeFx.xml");
	

}

void ScriptHitFx::Update(float timeStep) 
{
	
}

void ScriptHitFx::FixedUpdate(float timeStep)
{
	if (currentLifeTime_ > maxLifeTime) 
	{
		//Node* player = GetScene()->GetChild("playerNode", true);
		Node* camera = GetScene()->GetChild("cameraNode", true);

		Vector3 pos = GetNode()->GetWorldPosition();
		Vector3 camPos = camera->GetWorldPosition();

		Quaternion quat;
		//quat = GetNode()->GetRotation();
		quat.FromRotationTo(pos, camPos);
		//quat.FromLookRotation(camPos, camera->GetUp());
		
		
		
		smokeFX_= SharedPtr<Node>(GetNode()->GetScene()->InstantiateXML(prefabSmokeFx_->GetRoot(), pos, quat, LOCAL));
		smokeFX_->SetWorldScale(1.0f);
		ScriptSmokeFx* script = smokeFX_->CreateComponent<ScriptSmokeFx>();
		script->SetOrientationToNode(camera);

		GetNode()->Remove();
	}

	currentLifeTime_ += timeStep;
}
