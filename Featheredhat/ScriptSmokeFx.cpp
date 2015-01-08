#include "Common.h"
#include "ScriptSmokeFx.h"


ScriptSmokeFx::ScriptSmokeFx(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	//SetUpdateEventMask(USE_FIXEDUPDATE);
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);

}

void ScriptSmokeFx::RegisterObject(Context* context)
{
	context->RegisterFactory<ScriptSmokeFx>();
}

void ScriptSmokeFx::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	maxLifeTime = 1.0f; // Секунды
	currentLifeTime_ = 0.0f;

	scaleAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	
	float startSize = 1.0f * GetNode()->GetWorldScale().Length(); // relative on inited scale
	float endSize = 3.0f * GetNode()->GetWorldScale().Length();

	scaleAnim_->SetKeyFrame(0.0f, Vector3(startSize, startSize, startSize));
	scaleAnim_->SetKeyFrame(maxLifeTime, Vector3(endSize, endSize, endSize));

	GetNode()->SetScale(startSize);
	GetNode()->SetAttributeAnimation("Scale", scaleAnim_, WM_LOOP);

	StaticModel* model = GetComponent<StaticModel>();

#if 1
	mat_ = SharedPtr<Material>(model->GetMaterial(0)->Clone("ClonedMat"));
	//cache->AddManualResource(mat_);
#else
	mat_ = model->GetMaterial(0);
#endif

	// alpha factor
	factorAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	factorAnim_->SetKeyFrame(0.0f, 0.0f);
	factorAnim_->SetKeyFrame(maxLifeTime / 5.0f, 0.2f);
	factorAnim_->SetKeyFrame(maxLifeTime + (maxLifeTime / 5.0f), 1.0f); // litle shift (+1/5t) end value to remove visual artifacts with factor
	mat_->SetShaderParameterAnimation("Factor", factorAnim_);
	mat_->SetScene(GetScene());

	// color fade
	colorAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	colorAnim_->SetKeyFrame(0.0f, Vector4(1.0f,1.0f,1.0f, 1.0f));
	colorAnim_->SetKeyFrame((maxLifeTime / 3.0f), Vector4(1.0f,1.0f,1.0f, 1.0f));
	colorAnim_->SetKeyFrame(maxLifeTime + (maxLifeTime / 5.0f), Vector4(0.2f,0.2f,0.2f, 1.0f));
	mat_->SetShaderParameterAnimation("MatDiffColor", colorAnim_);
	mat_->SetScene(GetScene());

	model->SetMaterial(0,mat_);

	
	
	target_ = NULL;
}

void ScriptSmokeFx::Update(float timeStep) 
{
	if (target_) 
	{
		Vector3 targetPos = GetNode()->GetWorldPosition() - target_->GetWorldPosition();
		Quaternion smokeOrient;

		smokeOrient.FromRotationTo(GetNode()->GetWorldPosition(), targetPos);

		GetNode()->SetWorldRotation(smokeOrient);
	}
	


}

void ScriptSmokeFx::FixedUpdate(float timeStep)
{
	currentLifeTime_ += timeStep;

	if (currentLifeTime_ > maxLifeTime) 
	{
		//GetNode()->SetEnabled(false);
		GetNode()->Remove();
	}
}


void ScriptSmokeFx::SetOrientationToNode(Node *target) 
{
	target_ = target;
}