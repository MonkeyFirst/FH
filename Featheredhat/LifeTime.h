#pragma once

class LifeTime : public LogicComponent
{
	OBJECT(LifeTime);
public:

	/// Construct.
	LifeTime(Context* context);
	void SetLifeTime(float lifeTime);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);
	virtual void Start();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);


private:
	float currentLifeTime_;
	float maxLifeTime_;

};