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

//namespace Urho3D
//{
//	class Node;
//	class Scene;
//	class Plane;
//	class RigidBody;
//	class LogicComponent;
//	class Controls;
//	class Drawable;
//	class ValueAnimation;
//	class Material;
//	class StaticModel;
//	class AnimatedModel;
//	class AnimationState;
//	class Geometry;
//};
//
//using namespace Urho3D;

class ScriptHitFx : public LogicComponent 
{
	OBJECT(ScriptHitFx);
public:
	/// Construct.
	ScriptHitFx(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);

	float maxLifeTime;

private:
	float currentLifeTime_;
	SharedPtr<ValueAnimation> scaleAnim_;
	SharedPtr<XMLFile> prefabSmokeFx_;
	SharedPtr<Node> smokeFX_;

};