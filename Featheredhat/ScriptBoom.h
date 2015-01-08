#pragma once
//#include "Common.h"
//
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
//	class Geometry;
//};
//
//using namespace Urho3D;

class ScriptBoom : public LogicComponent 
{
	OBJECT(ScriptBoom);
public:
	/// Construct.
	ScriptBoom(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);
	
private:
	//SharedPtr<>;
	SharedPtr<Material> material_;
	SharedPtr<ValueAnimation> factorAnim_;
	SharedPtr<ValueAnimation> scaleAnim_;
	SharedPtr<ValueAnimation> lightAnim_;
	SharedPtr<Light> light_;
	SharedPtr<Node> ringNode_;
	SharedPtr<Material> ringMaterial_;
	SharedPtr<ValueAnimation> ringAlphaAnim_;

	float curTime_;
	
};