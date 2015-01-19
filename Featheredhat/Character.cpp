#include "Common.h"
#include "Character.h"



Character::Character(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);

}

void Character::RegisterObject(Context* context)
{
	context->RegisterFactory<Character>();
}

void Character::Start()
{
	// Component has been inserted into its scene node. Subscribe to events now
	SubscribeToEvent(GetNode(), E_NODECOLLISION, HANDLER(Character, HandleNodeCollision));
	


	rigidbody_ = GetNode()->GetComponent<RigidBody>();
	//rigidbody_->SetFriction(1.0f);
	cameraNode_ = GetNode()->GetScene()->GetChild("cameraNode", true);

	listener_ = cameraNode_->CreateComponent<SoundListener>();
	Audio* audio = GetSubsystem<Audio>();
	audio->SetListener(listener_);

	nodeBird_ = GetNode()->GetChild("BIRD", true);
	nodeGirl_ = nodeBird_->GetChild("GIRL", true);
	nodeGuns_ = nodeBird_->GetChild("GUNS", true);

	modelBird_ = nodeBird_->GetComponent<AnimatedModel>();
	modelGirl_ = nodeGirl_->GetComponent<AnimatedModel>();
	modelGuns_ = nodeGuns_->GetComponent<AnimatedModel>();

	FireFxJoint = modelGuns_->GetSkeleton().GetBone("defFireFx");
	FireFxJoint->node_->SetWorldScale(Vector3(0.001f, 0.001f, 0.001f));
	FireFxJoint->animated_ = false;
	
	animStateBirdIdle_ = modelBird_->GetAnimationState(StringHash("BIRD_FLYING"));
	animStateGirlIdle_ = modelGirl_->GetAnimationState(StringHash("GIRL_FLYING"));
	animStateGunsIdle_ = modelGuns_->GetAnimationState(StringHash("GUN_TB_FLYING"));

	animStateBirdForward_ = modelBird_->GetAnimationState(StringHash("BIRD_FLYING_FORWARD"));
	animStateGirlForward_ = modelGirl_->GetAnimationState(StringHash("GIRL_FLYING_FORWARD"));

	animStateBirdFire_ = modelBird_->GetAnimationState(StringHash("BIRD_FLYING_FIRE"));
	animStateGirlFire_ = modelGirl_->GetAnimationState(StringHash("GIRL_FLYING_FIRE"));
	animStateGunsFire_ = modelGuns_->GetAnimationState(StringHash("GUN_TB_FIRE"));


	animStateBirdIdle_->SetLooped(true);
	animStateGirlIdle_->SetLooped(true);
	animStateGunsIdle_->SetLooped(true);

	animStateBirdForward_->SetLooped(true);
	animStateGirlForward_->SetLooped(true);

	animStateBirdFire_->SetLooped(true);
	animStateGirlFire_->SetLooped(true);
	animStateGunsFire_->SetLooped(true);

	animStateBirdIdle_->SetTime(0.0f);
	animStateGirlIdle_->SetTime(0.0f);
	animStateGunsIdle_->SetTime(0.0f);

	animStateBirdForward_->SetTime(0.0f);
	animStateGirlForward_->SetTime(0.0f);

	animStateBirdFire_->SetTime(0.0f);
	animStateGirlFire_->SetTime(0.0f);
	animStateGunsFire_->SetTime(0.0f);


	animStateBirdIdle_->SetWeight(1.0f);
	animStateGirlIdle_->SetWeight(1.0f);
	animStateGunsIdle_->SetWeight(1.0f);

	animStateBirdForward_->SetWeight(0.0f);
	animStateGirlForward_->SetWeight(0.0f);

	animStateBirdFire_->SetWeight(0.0f);
	animStateGirlFire_->SetWeight(0.0f);
	animStateGunsFire_->SetWeight(0.0f);

	animStateBirdIdle_->SetLayer(0);
	animStateGirlIdle_->SetLayer(0);
	animStateGunsIdle_->SetLayer(0);

	animStateBirdForward_->SetLayer(1);
	animStateGirlForward_->SetLayer(1);

	animStateBirdFire_->SetLayer(2);
	animStateGirlFire_->SetLayer(2);
	animStateGunsFire_->SetLayer(2);


	// states setup

	states[CharacterState::ANI_FLYING_IDLE] = new CharacterFlyingIdle(this);
	states[CharacterState::ANI_FLYING_FORWARD] = new CharacterFlyingForward(this);
	states[CharacterState::ANI_FIRE_AUTO_GUN] = new CharacterFireWithAutoGun(this);
	

	animPrevState_ = CharacterState::ANI_NONE;
	state_ = states[CharacterState::ANI_FLYING_IDLE];


	SubscribeToEvent(nodeGuns_, E_ANIMATIONTRIGGER, HANDLER(Character, HandleAnimationTrigger));
	AutogunBulletJoint_ = nodeGuns_->GetChild("BulletSpawn", true);

	AutogunLight_ = nodeGuns_->GetChild("JointFireFx", true)->GetComponent<Light>();
	AutogunLight_->SetBrightness(0.0f);
	
}

void Character::ChangeState(CharacterState* newState)
{
	if (state_) state_->ExitState();
	state_ = newState;
	state_->EnterState();
	ClearPrevAnimStates();
}

void Character::ClearPrevAnimStates()
{
	if (animPrevState_ == CharacterState::ANI_NONE) return; // if nothing to erase 
	if (animPrevState_ == animState_) return;

	//turn-off previous anim state
	switch (animPrevState_)
	{
		case CharacterState::ANI_FLYING_IDLE:
		{
		
			//animStateBirdIdle_->SetWeight(0.0f);
			//animStateGirlIdle_->SetWeight(0.0f);
			//animStateGunsIdle_->SetWeight(0.0f);
			animStateBirdIdle_->SetTime(0.0f);
			animStateGirlIdle_->SetTime(0.0f);
			animStateGunsIdle_->SetTime(0.0f);

			break;
		}
		case CharacterState::ANI_FLYING_FORWARD:
		{
			//
			animStateBirdForward_->SetWeight(0.0f);
			animStateGirlForward_->SetWeight(0.0f);
			animStateBirdForward_->SetTime(0.0f);
			animStateGirlForward_->SetTime(0.0f);
			animStateGunsIdle_->SetTime(0.0f);

			break;
		}
		case CharacterState::ANI_FIRE_AUTO_GUN:
		{
			animStateBirdFire_->SetWeight(0.0f);
			animStateGirlFire_->SetWeight(0.0f);
			animStateGunsFire_->SetWeight(0.0f);
			animStateBirdFire_->SetTime(0.0f);
			animStateGirlFire_->SetTime(0.0f);
			animStateGunsFire_->SetTime(0.0f);
		}
	}

	animPrevState_ = CharacterState::ANI_NONE;
	
}

void Character::Update(float timeStep) 
{
	if (state_) state_->Update(timeStep);
}

void Character::FixedUpdate(float timeStep)
{
	static Vector3 lastDir = Vector3::RIGHT;
	/// \todo Could cache the components for faster access instead of finding them each frame
	RigidBody* body = GetComponent<RigidBody>();
	bool kW = false;
	bool kA = false;
	bool kS = false;
	bool kD = false;
	bool kSpace = false;
	bool kZ = false;


	const float MOVE_SPEED = 300.0f;

	Input* input = GetSubsystem<Input>();

	Quaternion worldRotation = cameraNode_->GetWorldRotation();
	//worldRotation.z_ = 0.0f;

	Vector3 oldVel = rigidbody_->GetLinearVelocity();

	Vector3 finalVelocity = Vector3::ZERO;

	if (input->GetKeyDown('W'))
	{
		kW = true;
		//cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		finalVelocity += worldRotation * Vector3::FORWARD * MOVE_SPEED;
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::FORWARD * MOVE_SPEED * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::FORWARD * MOVE_SPEED);

	}

	if (input->GetKeyDown('S'))
	{
		kS = true;
		//cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		finalVelocity += worldRotation * Vector3::BACK * (MOVE_SPEED / 2.0f);
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::BACK * (MOVE_SPEED / 2.0f) * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::BACK * (MOVE_SPEED / 2.0f));
	}

	if (input->GetKeyDown('A'))
	{
		kA = true;
		//cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		finalVelocity += worldRotation * Vector3::LEFT * (MOVE_SPEED / 2.0f);
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::LEFT * (MOVE_SPEED / 2.0f) * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::LEFT * (MOVE_SPEED / 2.0f));	
	}

	if (input->GetKeyDown('D'))
	{
		kD = true;
		//cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
		finalVelocity += worldRotation * Vector3::RIGHT * MOVE_SPEED;
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::RIGHT * MOVE_SPEED * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::RIGHT * MOVE_SPEED);
	}

	if (input->GetKeyDown('Z'))
	{
		kZ = true;
		//cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
		finalVelocity += worldRotation * Vector3::DOWN * (MOVE_SPEED / 2.0f);
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::RIGHT * MOVE_SPEED * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::RIGHT * MOVE_SPEED);
	}

	if (input->GetKeyDown(KEY_SPACE))
	{
		kSpace = true;
		finalVelocity += worldRotation * Vector3::UP * (MOVE_SPEED / 2.0f);
		//rigidbody_->SetLinearVelocity(worldRotation * Vector3::UP * MOVE_SPEED  * timeStep);
		//rigidbody_->ApplyForce(worldRotation * Vector3::UP * MOVE_SPEED * 5);
	}


	if (animState_ == CharacterState::ANI_FIRE_AUTO_GUN)
		finalVelocity *= 0.5f;

	if (finalVelocity.Length() > 0.1f)
		rigidbody_->SetLinearVelocity(finalVelocity * timeStep);


	static bool isAlradyFired = false;

	if (input->GetMouseButtonDown(MOUSEB_LEFT))
	{
		if (!isAlradyFired)
		{
			if (animState_ != CharacterState::ANI_FIRE_AUTO_GUN)
			{
				ChangeState(states[CharacterState::ANI_FIRE_AUTO_GUN]);
			}

			isAlradyFired = true;
		}
	}
	else
	{
		isAlradyFired = false;
	}

	static bool isTimeScaled = false;
	if (input->GetKeyDown(KEY_Q))
	{
		if (!isTimeScaled)
		{
			if (GetScene()->GetTimeScale() > 0.5f)
				GetNode()->GetScene()->SetTimeScale(0.1f);
			else
				GetScene()->SetTimeScale(1.0f);

			isTimeScaled = true;
		}
	}
	else
	{
		isTimeScaled = false;
	}

	// finalVelocity
	Vector3 speed = rigidbody_->GetLinearVelocity();
	if (speed.Length() > 1.0f) lastDir = speed.Normalized();

	Vector3 dir = lastDir;
	Vector3 planeDir = dir;
	planeDir.y_ = 0.0f;
	if (kW | kS) nodeBird_->SetWorldDirection(planeDir);

	if (animState_ != CharacterState::ANI_FIRE_AUTO_GUN)
	if (speed.Length() > 1.0f) 
	{
		if (animState_ != CharacterState::ANI_FLYING_FORWARD) 
		{
			ChangeState(states[CharacterState::ANI_FLYING_FORWARD]);
		}
	}
	else 
	{
		if (animState_ != CharacterState::ANI_FLYING_IDLE)
		{
			ChangeState(states[CharacterState::ANI_FLYING_IDLE]);
		}
	}

	// проверка света пулемета
	if (animState_ == CharacterState::ANI_FIRE_AUTO_GUN) 
	{
		float brightness = AutogunLight_->GetBrightness();
		if (brightness > 0.01f)
		{
			brightness -= 0.1f;
			AutogunLight_->SetBrightness(brightness);
		}
	}

	

}



void Character::HandleGlobalBlastEvent(StringHash eventType, VariantMap& eventData)
{

}

void Character::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
	// Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
	using namespace NodeCollision;

	Node* contact_node = (Node*)eventData[P_OTHERNODE].GetPtr();
	VectorBuffer contacts = eventData[P_CONTACTS].GetBuffer();
	Vector3 pos = contacts.ReadVector3(); // точка столкновени€

	Variant myAttr = contact_node->GetVar("type");
	if ( myAttr.GetString() == "value") 
	{

	}

}

// ќбработка событий с гильзами в анимации пулемЄта
void Character::HandleAnimationTrigger(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;
	//EVENT(E_ANIMATIONTRIGGER, AnimationTrigger)
	//{
	//	PARAM(P_NODE, Node);                    // Node pointer
	//	PARAM(P_NAME, Name);                    // String
	//	PARAM(P_TIME, Time);                    // Float
	//	PARAM(P_DATA, Data);                    // User-defined data type
	//}

	AnimationState* as = modelGuns_->GetAnimationState(StringHash(eventData[P_NAME].GetString()));

	if (as)
	{
		//String name = eventData[P_DATA].GetString();
		//Node* bone = nodeGuns_->GetChild(StringHash(name), true);
		Quaternion quatGuns = nodeGuns_->GetWorldRotation(); // ориентаци€ всего корпуса пушек
		
		Vector3 pos = AutogunBulletJoint_->GetWorldPosition(); // позици€ дл€ префаба
		Quaternion quat = AutogunBulletJoint_->GetWorldRotation(); // ориентаци€

		Node* gilza_ = GetScene()->InstantiateXML(gameworld_->prefabs.prefabAutogunBullet_->GetRoot(), pos, quat, LOCAL);
		gilza_->SetWorldScale(Vector3(0.11f, 0.11f, 0.11f));
		LifeTime* lt = gilza_->CreateComponent<LifeTime>();
		lt->SetLifeTime(5.0f);

		// дым от гильзы
		Node* smokeFx = GetScene()->InstantiateXML(gameworld_->prefabs.prefabBlackSmokeFx_->GetRoot(), pos, quat, LOCAL);
		smokeFx->SetParent(gilza_);
		smokeFx->SetPosition(Vector3(0, 0, 2));
		
		float rx = Random(0.0f, 360.0f);
		smokeFx->SetRotation(Quaternion(rx, rx, rx));
		smokeFx->SetScale(Vector3::ONE * 4.0f);
		smokeFx->CreateComponent<ScriptSmokeFx>();
		


		// инерци€ вылета гильзы в нужном направлении
		RigidBody* gilzaRigidbody_ = gilza_->GetComponent<RigidBody>();
		gilzaRigidbody_->SetLinearVelocity((quatGuns * (Vector3::UP + Vector3::RIGHT)) * 3.0f);
		gilzaRigidbody_->SetAngularVelocity(Vector3(-1.0f, 0.1f, 0.0f));
		AutogunLight_->SetBrightness(2.0f);
		
		


		// RayCast Forward
		Vector3 hitPos;
		Vector3 hitNormal;
		Drawable* hitDrawable;
		bool hit = false;
		if (Raycast(100.0f, hitPos, hitNormal, hitDrawable))
		{
			hit = true;
			Quaternion q;
			q.FromRotationTo(hitPos, hitNormal);
			Node* hitFx = GetScene()->InstantiateXML(gameworld_->prefabs.prefabHitFx1_->GetRoot(), hitPos, q, LOCAL);
			hitFx->SetWorldScale(Vector3::ONE * 0.5f);
			hitFx->CreateComponent<ScriptHitFx>();
			
			

			

			//SharedPtr<SoundSource3D> source = SharedPtr<SoundSource3D>(hitFx->CreateComponent<SoundSource3D>());
			//source->Play(gameworld_->sfx.hitSound);
			//source->SetFarDistance(20.0f);
			//source->SetAutoRemove(true);
			Variant v = hitDrawable->GetNode()->GetVar("type");
			if (v.GetString() == "r2bot") 
			{
				//BotAI* ai = hitDrawable->GetNode()->GetComponent<BotAI>();
				//if (ai) ai->Hit();
				VariantMap eventData;
				Node* node = hitDrawable->GetNode();
				eventData[P_DATA] = node->GetParent();
				GetNode()->SendEvent("YouAreHit", eventData);
			}
		}


		//TailLine
		Node* gunNode_ = nodeGuns_->GetChild("JointFireFx", true);

		Node* TailNode = GetScene()->CreateChild("tileNode", LOCAL);
		TailNode->SetPosition(Vector3::ZERO);
		TailNode->SetScale(Vector3::ONE);

		ScriptTailLine* st = TailNode->CreateComponent<ScriptTailLine>();
		
		Vector3 tileEnd = gunNode_->GetWorldPosition() + (gunNode_->GetWorldRotation() * (Vector3::UP * 20.0f));

		if (hit) tileEnd = hitPos;

		st->SetupTailLine(gunNode_->GetWorldPosition(), tileEnd, 0.5f, 0.2f, ScriptTailLine::WHITE); // start and end line + sec

	}
}

bool Character::Raycast(float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable)
{

	hitDrawable = 0;

	//Graphics* graphics = GetSubsystem<Graphics>();
	Node* gunNode_ = nodeGuns_->GetChild("JointFireFx", true);

	Ray gunRay = Ray(gunNode_->GetWorldPosition(), nodeGuns_->GetWorldDirection());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, gunRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY, 191); // all except 7 bit (layer for fx)

	Octree* octree = GetScene()->GetComponent<Octree>();
	octree->Raycast(query);

	if (results.Size())
	{
		for (unsigned int i = 0; i < results.Size(); i++)
		{
			RayQueryResult& result = results[i];

			Variant fx = result.node_->GetVar("fx");
			if (fx.GetInt() == 1) continue;

			hitPos = result.position_;
			hitNormal = result.normal_;
			hitDrawable = result.drawable_;
			


			return true;
		}
	}

	return false;
}


/************************************************************************/
/* STATES                                                               */
/************************************************************************/

void CharacterState::EnterState() // call once then new state are set 
{
	playedTime_ = 0.0f;
	character_->animState_ = stateAnimation_;

};
void CharacterState::ExitState() // call once then state off
{
	character_->animPrevState_ = stateAnimation_;
};


void CharacterFlyingIdle::Update(float timeStep) 
{
	character_->animStateBirdIdle_->SetWeight(1.0f);
	character_->animStateGirlIdle_->SetWeight(1.0f);
	character_->animStateGunsIdle_->SetWeight(1.0f);

	character_->animStateBirdIdle_->AddTime(timeStep);
	character_->animStateGirlIdle_->AddTime(timeStep);
	character_->animStateGunsIdle_->AddTime(timeStep);


	float t = character_->animStateBirdIdle_->GetTime();
	float l = character_->animStateBirdIdle_->GetLength() - timeStep;

	if (t >= l)
	{
		// restart animations
		character_->animStateBirdIdle_->SetTime(0.0f); 
		character_->animStateGirlIdle_->SetTime(0.0f); 
		character_->animStateGunsIdle_->SetTime(0.0f); 
	}

}

void CharacterFlyingIdle::FixedUpdate(float timeStep) 
{

}

//////////////////////////////////////////////////////////////////////////

void CharacterFlyingForward::Update(float timeStep)
{
	character_->animStateBirdForward_->SetWeight(1.0f);
	character_->animStateGirlForward_->SetWeight(1.0f);
	character_->animStateGunsIdle_->SetWeight(1.0f);

	character_->animStateBirdForward_->AddTime(timeStep);
	character_->animStateGirlForward_->AddTime(timeStep);
	character_->animStateGunsIdle_->AddTime(timeStep);

	float t = character_->animStateBirdForward_->GetTime();
	float l = character_->animStateBirdForward_->GetLength() - timeStep;

	if (t >= l)
	{
		// restart animations
		character_->animStateBirdForward_->SetTime(0.0f);
		character_->animStateGirlForward_->SetTime(0.0f);
		character_->animStateGunsIdle_->SetTime(0.0f);
	}

}

void CharacterFlyingForward::FixedUpdate(float timeStep)
{

}

//////////////////////////////////////////////////////////////////////////

void CharacterFireWithAutoGun::Update(float timeStep) 
{
	character_->animStateBirdFire_->SetWeight(1.0f);
	character_->animStateGirlFire_->SetWeight(1.0f);
	character_->animStateGunsFire_->SetWeight(1.0f);

	character_->animStateBirdFire_->AddTime(timeStep);
	character_->animStateGirlFire_->AddTime(timeStep);
	character_->animStateGunsFire_->AddTime(timeStep);

	float t = character_->animStateBirdFire_->GetTime();
	float l = character_->animStateBirdFire_->GetLength() - timeStep;

	if (t >= l) // если это конец анимации с выстрелами сбросим врем€ и переключим стейт перса
	{
		// if this state ends reset times and change character state to idle
		character_->animStateBirdFire_->SetTime(0.0f);
		character_->animStateGirlFire_->SetTime(0.0f);
		character_->animStateGunsFire_->SetTime(0.0f);

		character_->ChangeState(character_->states[CharacterState::ANI_FLYING_IDLE]);
	}
}

void CharacterFireWithAutoGun::FixedUpdate(float timeStep) 
{

}

void CharacterFireWithAutoGun::EnterState() // call once then new state are set 
{
	CharacterState::EnterState();
	// в анимации со стрельбой активируем кость с которой св€зан эффект огн€ от выстрела
	character_->FireFxJoint->animated_ = true;
	character_->FireFxJoint->node_->SetWorldScale(Vector3::ONE);
};
void CharacterFireWithAutoGun::ExitState() // call once then state off
{
	// при выходе из этой анимации в другие спр€чем сетку огн€
	character_->FireFxJoint->animated_ = false;
	character_->FireFxJoint->node_->SetWorldScale(Vector3(0.001f, 0.001f, 0.001f));
	CharacterState::ExitState();
};