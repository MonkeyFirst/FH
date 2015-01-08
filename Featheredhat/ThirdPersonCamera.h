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
//#include <Urho3D/Physics/RigidBody.h>
//#include <Urho3D/Graphics/Octree.h>
//
//namespace Urho3D
//{
//	class Node;
//	class Scene;
//	class Plane;
//	class RigidBody;
//	class LogicComponent;
//	class Controls;
//	class Drawable;
//	class Octree;
//
//};
//
//using namespace Urho3D;

class ThirdPersonCamera: public LogicComponent 
{
	OBJECT(ThirdPersonCamera);
public:
	/// Construct.
	ThirdPersonCamera(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);

	virtual void Start();
	void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);
	
	void SetTargetNode(Node* target);
	void SetMinMaxDistance(float minDistance, float maxDistance);

	void HandleGlobalBlastEvent(StringHash eventType, VariantMap& eventData);
	
private:

	SharedPtr<Node> target_;
	SharedPtr<Node> shakeNode_;
	SharedPtr<Node> angleNode_;
	SharedPtr<Node> cameraNode_;
	SharedPtr<Camera> camera_;

	int wheel_;
	float lastCollisionTime;

	float minFollow_;
	float maxFollow_;

	float pitch_;
	float yaw_;
	float follow_;
	float curFollow_;
	float followVel_;
	Vector3 pos;
	Vector3 newPos;
	Vector3 posVelocity;

	float shakeMagnitude;
	float shakeSpeed;
	float shakeTime;
	float shakeDamping;

	void SetShake(float magnitude, float speed, float damping);
	void SpringFollow(float timeStep);
	void SpringPosition(float timeStep);
	float CameraTestObstacles(float followDistance, bool& hasObstacle);


};