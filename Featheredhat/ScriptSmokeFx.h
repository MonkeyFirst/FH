#pragma once
//#include "Common.h"

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


class ScriptSmokeFx : public LogicComponent 
{
	OBJECT(ScriptSmokeFx);
public:
	/// Construct.
	ScriptSmokeFx(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);
	void SetOrientationToNode(Node *target);
	void SetScaleFactor(float scaleFactor_);

	float maxLifeTime;

private:
	float currentLifeTime_;
	SharedPtr<ValueAnimation> scaleAnim_;
	SharedPtr<ValueAnimation> factorAnim_;
	SharedPtr<ValueAnimation> colorAnim_;
	SharedPtr<Material> mat_;
	WeakPtr<Node> target_;

	float startScale;

};