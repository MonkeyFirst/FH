#include "Common.h"
#include "ScriptBoom.h"


ScriptBoom::ScriptBoom(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE);
	
}

void ScriptBoom::RegisterObject(Context* context)
{
	context->RegisterFactory<ScriptBoom>();
}

void ScriptBoom::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>(); 
	material_ = cache->GetResource<Material>("Materials/MT_Boom.xml");
	

	ringNode_ = GetNode()->GetChild("boomRing",true);
	StaticModel* ringsModel = ringNode_->GetComponent<StaticModel>();
	Geometry* g = ringsModel->GetLodGeometry(0,0);
	

	// AlphaMask Factor
	factorAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	
	//srand(0);
	//float noise = Random(0.1f, 0.5f);
	//float lag = Random(-0.5f, 0.5f);

	factorAnim_->SetKeyFrame(0.0f, 0.5f);
	factorAnim_->SetKeyFrame(0.5f, 1.0f);
	factorAnim_->SetKeyFrame(1.0f, 2.0f);
	
	material_->SetShaderParameterAnimation("Factor", factorAnim_);
	material_->SetScene(GetScene());

	// Scale
	scaleAnim_ = SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	

	float startSize = 0.1f;
	float endSize = 2.0f;
	
	scaleAnim_->SetKeyFrame(0.0f, Vector3(startSize, startSize, startSize));
	scaleAnim_->SetKeyFrame(0.5f, Vector3(endSize, endSize, endSize));
	scaleAnim_->SetKeyFrame(1.0f, Vector3(startSize, startSize, startSize));
	
	GetNode()->SetScale(startSize);

	GetNode()->SetAttributeAnimation("Scale", scaleAnim_, WM_LOOP);
	
	
	//light
	light_ = GetNode()->GetComponent<Light>();
	
	lightAnim_ =  SharedPtr<ValueAnimation>(new ValueAnimation(context_));
	
	lightAnim_->SetKeyFrame(0.0f, 0.0f);
	lightAnim_->SetKeyFrame(0.5f, 10.0f);
	lightAnim_->SetKeyFrame(1.0f, 0.0f);

	light_->SetAttributeAnimation("Brightness Multiplier", lightAnim_);

	// ring
	ringMaterial_ =  cache->GetResource<Material>("Materials/MT_BoomRing.xml");
	

	ringAlphaAnim_ =  SharedPtr<ValueAnimation>(new ValueAnimation(context_));

	ringAlphaAnim_->SetKeyFrame(0.0f, Vector4(1.0f,1.0f,1.0f, 0.0f)); // no visible rings
	ringAlphaAnim_->SetKeyFrame(0.5f, Vector4(1.0f,1.0f,1.0f, 1.0f)); // over visible 
	ringAlphaAnim_->SetKeyFrame(0.6f, Vector4(1.0f,1.0f,1.0f, 0.2f)); // almost visible
	ringAlphaAnim_->SetKeyFrame(1.0f, Vector4(1.0f,1.0f,1.0f, 0.0f)); // no visible

	ringMaterial_->SetShaderParameterAnimation("MatDiffColor", ringAlphaAnim_);
	ringMaterial_->SetScene(GetScene());

	curTime_ = 0.0f;
	
}

void ScriptBoom::Update(float timeStep) 
{

}

void ScriptBoom::FixedUpdate(float timeStep)
{

	Vector3 s = GetNode()->GetWorldScale();
	
	if (s.Length() >= 1.9f) 
	{
		GetNode()->Remove();
		return;
	}
	curTime_ +=timeStep;
}
