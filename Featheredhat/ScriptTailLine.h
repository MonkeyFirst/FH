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


//namespace Urho3D
//{
//	class Node;
//	class Scene;
//	class Plane;
//	class RigidBody;
//	class LogicComponent;
//	class Controls;
//	class Drawable;
//	class VertexBuffer;
//	class IndexBuffer;
//	class Geometry;
//	class Model;
//
//};

//using namespace Urho3D;


typedef struct DYN_VERTEX
{
	Vector3 p;
	Vector3 n;
	Vector2 uv;
} DYN_VERTEX;

class ScriptTailLine : public LogicComponent
{
	OBJECT(ScriptTailLine);
public:
	enum TailMaterial {WHITE, RED};

	/// Construct.
	ScriptTailLine(Context* context);
	void SetupTailLine(Vector3 start, Vector3 end, float lifeTime, float width, TailMaterial tm);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	
	
private:
	float currentLifeTime_;

	Vector3 start_;
	Vector3 end_;
	float lifeTime_;

	PODVector<DYN_VERTEX> tailMesh_;

	SharedPtr<Model> fromScratchModel;
	SharedPtr<VertexBuffer> vb;
	SharedPtr<IndexBuffer> ib;
	SharedPtr<Geometry> geom;

	
};