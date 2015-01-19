#include "Common.h"
#include "BotAI.h"

BotAI::BotAI(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);
}

void BotAI::RegisterObject(Context* context)
{
	context->RegisterFactory<BotAI>();
}

void BotAI::SetGameWorld(struct GameWorld& gameWorld)
{
	gameWorld_ = &gameWorld;
}

void BotAI::Stop()
{
	gameWorld_ = NULL;

	/*for (BotAIState* p = states[0]; p != NULL, p++)
	{
	delete p;
	}*/

	state_ = NULL;
	if (states[BotAIState::ANI_IDLE_1]) delete states[BotAIState::ANI_IDLE_1];
	if (states[BotAIState::ANI_IDLE_2]) delete states[BotAIState::ANI_IDLE_2];
	if (states[BotAIState::ANI_IDLE_3]) delete states[BotAIState::ANI_IDLE_3];
	if (states[BotAIState::ANI_IDLE_4]) delete states[BotAIState::ANI_IDLE_4];
	if (states[BotAIState::ANI_IDLE_5]) delete states[BotAIState::ANI_IDLE_5];
	if (states[BotAIState::ANI_WALK]) delete states[BotAIState::ANI_WALK];
	if (states[BotAIState::ANI_FIRE]) delete states[BotAIState::ANI_FIRE];
	if (states[BotAIState::ANI_FALLING]) delete states[BotAIState::ANI_FALLING];

}

void BotAI::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	// get helpers
	node_ = GetNode()->GetChild("AnimMesh", true);
	rayNode_ = node_->GetChild("rayNode", true);
	fireNode_ = node_->GetChild("FireJoint", true);
	footL_ = node_->GetChild("footL", true);
	footR_ = node_->GetChild("footR", true);
	
	// get components
	rigidbody_ = node_->GetComponent<RigidBody>();
	animModel_ = node_->GetComponent<AnimatedModel>();
	animController_ = node_->GetComponent<AnimationController>();
	boneCenter_ = animModel_->GetSkeleton().GetBone("Center");
	//boneCenter_->animated_ = false;


	// get animations
	animIdle1_ = animModel_->GetAnimationState(StringHash("R2_IDLE_1"));
	animIdle2_ = animModel_->GetAnimationState(StringHash("R2_IDLE_2"));
	animIdle3_ = animModel_->GetAnimationState(StringHash("R2_IDLE_3"));
	animIdle4_ = animModel_->GetAnimationState(StringHash("R2_IDLE_4"));
	animIdle5_ = animModel_->GetAnimationState(StringHash("R2_IDLE_5"));
	animWalk_ = animModel_->GetAnimationState(StringHash("R2_WALK"));
	animFalling_ = animModel_->GetAnimationState(StringHash("R2_FALLING"));
	animFire_ = animModel_->GetAnimationState(StringHash("R2_FIRE"));

	// setup
	animIdle1_->SetLooped(true);
	animIdle2_->SetLooped(true);
	animIdle3_->SetLooped(true);
	animIdle4_->SetLooped(true);
	animIdle5_->SetLooped(true);
	animWalk_->SetLooped(true);
	animFalling_->SetLooped(false);
	animFire_->SetLooped(true);

	animIdle1_->SetWeight(1.0f);
	animIdle2_->SetWeight(0.0f);
	animIdle3_->SetWeight(0.0f);
	animIdle4_->SetWeight(0.0f);
	animIdle5_->SetWeight(0.0f);
	animWalk_->SetWeight(0.0f);
	animFalling_->SetWeight(0.0f);
	animFire_->SetWeight(0.0f);

	animIdle1_->SetLayer(1);
	animIdle2_->SetLayer(2);
	animIdle3_->SetLayer(3);
	animIdle4_->SetLayer(4);
	animIdle5_->SetLayer(5);
	animWalk_->SetLayer(6);
	animFalling_->SetLayer(7);
	animFire_->SetLayer(8);

	aiPoints = GetNode()->GetChild("AIPoints");
	aiPoints->GetChildren(checkPoints, false);


	SubscribeToEvent(node_, E_ANIMATIONTRIGGER, HANDLER(BotAI, HandleAnimationTrigger));
	SubscribeToEvent("AnimationEnds", HANDLER(BotAI, HandleAnimationEnds));
	SubscribeToEvent("BotDestroy", HANDLER(BotAI, HandleBotDestroy));
	SubscribeToEvent("CheckPoint", HANDLER(BotAI, HandleCheckPoint));
	SubscribeToEvent(StringHash("YouAreHit"), HANDLER(BotAI, HandleHit));


	
	//Add states
	states[BotAIState::ANI_IDLE_1] = new BotAIStateIdle1(this);
	states[BotAIState::ANI_IDLE_2] = new BotAIStateIdle2(this);
	states[BotAIState::ANI_IDLE_3] = new BotAIStateIdle3(this);
	states[BotAIState::ANI_IDLE_4] = new BotAIStateIdle4(this);
	states[BotAIState::ANI_IDLE_5] = new BotAIStateIdle5(this);
	states[BotAIState::ANI_WALK] = new BotAIStateCheckPoint(this);
	states[BotAIState::ANI_FIRE] = new BotAIStateFire(this);
	states[BotAIState::ANI_FALLING] = new BotAIStateFalling(this);


	animPrevState_ = BotAIState::ANI_NONE;
	state_ = states[BotAIState::ANI_IDLE_1];
	isRemoved = false;


	//sfx
	sfxRunSource_ = node_->CreateComponent<SoundSource3D>();
	sfxRunSource_->SetFarDistance(ATTACKRANGE*4);
	sfxRunSource_->SetGain(0.5f);
	sfxRunSource_->SetDistanceAttenuation(0, ATTACKRANGE * 4, 4.0f);
	

}

void BotAI::Update(float timeStep)
{
	if (isRemoved) return;

	if (state_) state_->Update(timeStep);
}

void BotAI::FixedUpdate(float timeStep)
{
	if (isRemoved) return;

	Vector3 velocity = rigidbody_->GetLinearVelocity();
	float speed = velocity.Length();

	if (speed > 0.2f && !isBotOnGround() && state_->stateType_ != BotAIState::STATE_FALLING)
	{
		ChangeState(states[BotAIState::ANIMStates::ANI_FALLING]);
	}
	else 
	{
		if ( state_->stateAni_ != BotAIState::ANI_FALLING ) CheckForFireByPlayer(ATTACKRANGE);
		if ( state_->stateAni_ != BotAIState::ANI_FIRE ) ProcessRandomCheckPoint(timeStep);	
		if ( state_->stateAni_ == BotAIState::ANI_WALK ) ProcessEmmitFootSteps(timeStep);
	}

	if (state_) state_->FixedUpdate(timeStep);
}

void BotAI::HandleHit(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;
	if (!isRemoved)
	{

		if (GetNode() == (Node*)eventData[P_DATA].GetVoidPtr())
		{
			DestroyBot();
		}
	}
}

void BotAI::Hit()
{
	static int hitCount = 0;
	hitCount++;
	if (hitCount >= 3)
	{
		if (!isRemoved) DestroyBot();
	}
}

void BotAI::HandleAnimationTrigger(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;
	//EVENT(E_ANIMATIONTRIGGER, AnimationTrigger)
	//{
	//	PARAM(P_NODE, Node);                    // Node pointer
	//	PARAM(P_NAME, Name);                    // String
	//	PARAM(P_TIME, Time);                    // Float
	//	PARAM(P_DATA, Data);                    // User-defined data type
	//}

	AnimationState* as = animModel_->GetAnimationState(StringHash(eventData[P_NAME].GetString()));

	if (as)
	{
		String name = eventData[P_DATA].GetString();
		float time = eventData[P_TIME].GetFloat();

		Node* bone = node_->GetChild(StringHash(name), true);
		Vector3 pos = fireNode_->GetWorldPosition();
		Quaternion quat = fireNode_->GetWorldRotation();

		// instantiate fire fx
		{

			SharedPtr<Node> fireFX_ = SharedPtr<Node>(GetNode()->GetScene()->InstantiateXML(gameWorld_->prefabs.prefabFireFx_->GetRoot(), pos, quat, LOCAL));
			fireFX_->SetWorldScale(Vector3::ONE / 4.0f);
			ScriptFireFx* script = fireFX_->CreateComponent<ScriptFireFx>();
			script->sfxFireSource_->Play(gameWorld_->sfx.botFireSound);
			script->sfxFireSource_->SetAutoRemove(true);
		}

		// instantiate TailLine
		
		Node* TailNode = GetScene()->CreateChild("botTileLine", LOCAL);
		TailNode->SetPosition(Vector3::ZERO);
		TailNode->SetScale(Vector3::ONE);
		ScriptTailLine* st = TailNode->CreateComponent<ScriptTailLine>();
		Vector3 tileEnd = pos + (quat * (Vector3::FORWARD * 10.0f));
		st->SetupTailLine(pos, tileEnd, 1.0f, 0.1f, ScriptTailLine::WHITE); // start and end line + sec

		
		// instantiate smoke at end of tail line
		Node* smokeFX_ = GetNode()->GetScene()->InstantiateXML(gameWorld_->prefabs.prefabSmokeFx_->GetRoot(), tileEnd, quat, LOCAL);
		ScriptSmokeFx* script = smokeFX_->CreateComponent<ScriptSmokeFx>();
		script->SetOrientationToNode(gameWorld_->camera.node_);

		smokeFX_->SetWorldPosition(tileEnd);
		smokeFX_->SetWorldScale(Vector3::ONE);




	}
}

void BotAI::HandleAnimationEnds(StringHash eventType, VariantMap& eventData)
{
	if (state_)
	if (eventData["BOT"].GetPtr() == this)
	if (state_->stateType_ == BotAIState::STATE_IDLE) 
	{
		BotAIState::ANIMStates r = (BotAIState::ANIMStates)Random((int)BotAIState::ANIMStates::ANI_IDLE_1, (int)BotAIState::ANIMStates::ANI_IDLE_5);
		ChangeState(states[r]);
	}
}

void BotAI::HandleBotDestroy(StringHash eventType, VariantMap& eventData)
{
	//if (eventData["BOT"].GetPtr() == this)
	if (state_ == states[BotAIState::ANIMStates::ANI_FALLING])
	{
		DestroyBot();
	}
}

void BotAI::HandleCheckPoint(StringHash eventType, VariantMap& eventData) 
{
	if (eventData["BOT"].GetPtr() == this)
	if (state_ == states[BotAIState::ANIMStates::ANI_WALK])
	{
		ChangeState(states[BotAIState::ANI_IDLE_1]);
	}
}

void BotAI::DestroyBot()
{
	isRemoved = true;
	Vector3 pos = node_->GetWorldPosition();
	Quaternion quat = node_->GetWorldRotation();

	SharedPtr<Node> boomFX_ = SharedPtr<Node>(GetNode()->GetScene()->InstantiateXML(gameWorld_->prefabs.prefabBoomFx_->GetRoot(), pos, quat, LOCAL));
	ScriptBoom* script = boomFX_->CreateComponent<ScriptBoom>();
	GetNode()->Remove();
}

void BotAI::ChangeState(BotAIState* newState)
{
	if (state_) state_->ExitState();
	state_ = newState;
	state_->EnterState();
	ClearPrevAnimStates();
}

void BotAI::ClearPrevAnimStates() 
{
	if (animPrevState_ == BotAIState::ANI_NONE) return; // if nothing to erase 
	if (animPrevState_ == animState_) return;

	//turn-off previous anim state
	switch (animPrevState_)
	{
		case BotAIState::ANI_IDLE_1:
		{

            //animIdle1_->SetWeight(0.0f); // this lower priority state it's always on
            animIdle1_->SetTime(0.0);

            break;
		}
		case BotAIState::ANI_IDLE_2:
		{
            animIdle2_->SetWeight(0.0f);
            animIdle2_->SetTime(0.0);
            break;
		}
		case BotAIState::ANI_IDLE_3:
		{
            animIdle3_->SetWeight(0.0f);
            animIdle3_->SetTime(0.0);
            break;
		}
		case BotAIState::ANI_IDLE_4:
		{
            animIdle4_->SetWeight(0.0f);
            animIdle4_->SetTime(0.0);
            break;
		}
		case BotAIState::ANI_IDLE_5:
		{
            animIdle5_->SetWeight(0.0f);
            animIdle5_->SetTime(0.0);
            break;
		}
		case BotAIState::ANI_WALK:
		{
            animWalk_->SetWeight(0.0f);
            animWalk_->SetTime(0.0);
            break;
		}
		case BotAIState::ANI_FIRE:
		{
            animFire_->SetWeight(0.0f);
            animFire_->SetTime(0.0f);
            break;
		}
		case BotAIState::ANI_FALLING:
		{
            animFalling_->SetWeight(0.0f);
            animFalling_->SetTime(0.0f);
            break;
		}
	}

	animPrevState_ = BotAIState::ANI_NONE;
}


bool BotAI::isBotOnGround()
{
	Ray ray(rayNode_->GetWorldPosition(), Vector3(0.0f, -1.0f, 0.0f));

	Vector3 hitPos;
	Vector3 hitNormal;
	Drawable* drawable;

	return Raycast(ray, 2.0f, hitPos, hitNormal, drawable);

}

bool BotAI::Raycast(Ray& ray, float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable)
{
	hitDrawable = NULL;
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, ray, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY, -1);

	Octree* octree = GetNode()->GetScene()->GetComponent<Octree>();
	//octree->RaycastSingle(query);
	octree->Raycast(query);

	if (results.Size())
	{
		for (unsigned int i = 0; i < results.Size(); i++) 
		{
			RayQueryResult& result = results[i];
			Variant fx = result.node_->GetVar("fx");
			Variant land = result.node_->GetVar("land");
			if (fx.GetInt() == 1) continue;
			if (land.GetInt() == 1) 
			{
				hitPos = result.position_;
				hitNormal = result.normal_;
				hitDrawable = result.drawable_;
				return true;
			}
		}
	}

	return false;
}

void BotAI::ProcessRandomCheckPoint(float timeStep)
{
	static float timeToNextAction = 0.0f;
	static float curTime = 0.0f;

	if (curTime >= timeToNextAction)
	{
		float delta = Random(2.0f, 5.0f);
		timeToNextAction += delta;

		float variant = Random(0.0f, 1.0f);

		if (variant > 0.75f)
		{
			selectedCheckPoint = GetRandomCheckPoint();
			ChangeState(states[BotAIState::ANI_WALK]);
		}

	}

	curTime += timeStep;
}

Node* BotAI::GetRandomCheckPoint()
{
	static Node* prevNode = NULL;
	Node * selected = NULL;
	int max = checkPoints.Size();

	do
	{
		int i = Random(0, max);

		selected = checkPoints[i];

	} while (prevNode == selected);
	prevNode = selected;

	return selected;
}

void BotAI::ProcessEmmitFootSteps(float timeStep)
{
	static bool emmitFootL = false;
	static bool emmitFootR = false;
	static float timeFromLastFootStep = 0.0f;
	float walkTime = animWalk_->GetTime();

	float timeL = 15.0f / 24.0f;
	float timeR = 5.0f / 24.0f;

	if (walkTime > timeL && walkTime < timeL + 0.1f)
	{
		emmitFootL = true;
		emmitFootR = false;
	}

	if (walkTime > timeR && walkTime < timeR + 0.1f)
	{
		emmitFootL = false;
		emmitFootR = true;
	}

	if (timeFromLastFootStep > 0.3f) // delay
	if (emmitFootL == true || emmitFootR == true)
	{
		Node* camera = gameWorld_->camera.node_;
		Vector3 pos;

		if (emmitFootL)
			pos = footL_->GetWorldPosition();
		else
			pos = footR_->GetWorldPosition();

		Vector3 camPos = gameWorld_->camera.node_->GetWorldPosition();
		Quaternion quat;
		quat.FromRotationTo(pos, camPos);
		SharedPtr<Node> smokeFX_ = SharedPtr<Node>(GetNode()->GetScene()->InstantiateXML(gameWorld_->prefabs.prefabSmokeFx_->GetRoot(), pos, quat, LOCAL));
		smokeFX_->SetWorldScale(Vector3::ONE * 0.5f);
		ScriptSmokeFx* script = smokeFX_->CreateComponent<ScriptSmokeFx>();
		//smokeFX_->SetWorldScale(Vector3::ONE * 0.05f);
		script->SetOrientationToNode(camera);

		timeFromLastFootStep = 0.0f;

	}

	timeFromLastFootStep += timeStep;

}


void BotAI::CheckForFireByPlayer(float range_)
{
	Vector3 botPos = node_->GetWorldPosition();
	Vector3 playerPos = gameWorld_->player.node_->GetWorldPosition();
	float distanceBetween = (botPos - playerPos).Length();

	float distanceByY = (botPos.y_ - playerPos.y_);


	if (distanceBetween < range_ && distanceByY < 0.0f) // player in range, go fire
	{
		target_ = gameWorld_->player.node_;
		ChangeState(states[BotAIState::ANI_FIRE]);
		isAttacking = true;
	}
	isAttacking = false;
}

///
/// BOT AI STATES
///
BotAIState::BotAIState() {};
BotAIState::BotAIState(BotAI& bot) { botAi_ = &bot; };
void BotAIState::SetActor(BotAI* bot) 
{ 
	botAi_ = bot; 
};
void BotAIState::EnterState() // call once then new state are set 
{ 
	playedTime_ = 0.0f;
	botAi_->animState_ = stateAni_;
	
};
void BotAIState::ExitState() // call once then state off
{
	botAi_->animPrevState_ = stateAni_;
};





///
/// IDLE STATE_1
///
void BotAIStateIdle1::Update(float timeStep) 
{
	botAi_->animIdle1_->SetWeight(1.0f);
	botAi_->animIdle1_->AddTime(timeStep);
	
	float t = botAi_->animIdle1_->GetTime();
	float l = botAi_->animIdle1_->GetLength() - timeStep;
	
	if ( t >= l )
	{
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("AnimationEnds", eventData);
	}
}

void BotAIStateIdle1::FixedUpdate(float timeStep) 
{
}

///
/// IDLE STATE_2
///
void BotAIStateIdle2::Update(float timeStep)
{
	botAi_->animIdle2_->SetWeight(1.0f);
	botAi_->animIdle2_->AddTime(timeStep);

	float t = botAi_->animIdle2_->GetTime();
	float l = botAi_->animIdle2_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("AnimationEnds", eventData);
	}

}

void BotAIStateIdle2::FixedUpdate(float timeStep)
{

}

///
/// IDLE STATE_3
///
void BotAIStateIdle3::Update(float timeStep)
{
	botAi_->animIdle3_->SetWeight(1.0f);
	botAi_->animIdle3_->AddTime(timeStep);

	float t = botAi_->animIdle3_->GetTime();
	float l = botAi_->animIdle3_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("AnimationEnds", eventData);
	}

}

void BotAIStateIdle3::FixedUpdate(float timeStep)
{
}

///
/// IDLE STATE_4
///
void BotAIStateIdle4::Update(float timeStep)
{
	botAi_->animIdle4_->SetWeight(1.0f);
	botAi_->animIdle4_->AddTime(timeStep);

	float t = botAi_->animIdle4_->GetTime();
	float l = botAi_->animIdle4_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("AnimationEnds", eventData);
	}

}

void BotAIStateIdle4::FixedUpdate(float timeStep)
{
}

///
/// IDLE STATE_5
///
void BotAIStateIdle5::Update(float timeStep)
{
	botAi_->animIdle5_->SetWeight(1.0f);
	botAi_->animIdle5_->AddTime(timeStep);

	float t = botAi_->animIdle5_->GetTime();
	float l = botAi_->animIdle5_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("AnimationEnds", eventData);
	}

}

void BotAIStateIdle5::FixedUpdate(float timeStep)
{
}

///
/// CHECKPOINTS STATE
///
void BotAIStateCheckPoint::Update(float timeStep)
{
	botAi_->animWalk_->SetWeight(1.0f);
	botAi_->animWalk_->AddTime(timeStep);

	float t = botAi_->animWalk_->GetTime();
	float l = botAi_->animWalk_->GetLength() - timeStep;

	if (t >= l)
	{
		//botAi_->animWalk_->SetTime(0.0f); // restart anim
	}
}

void BotAIStateCheckPoint::FixedUpdate(float timeStep)
{
	Vector3 botPos = botAi_->node_->GetWorldPosition();
	Vector3 pointPos = botAi_->selectedCheckPoint->GetWorldPosition();
	float distanceBetween = (botPos - pointPos).Length();

	if (distanceBetween > 1.0f)
	{
		Vector3 dir = (pointPos - botPos);
		Vector3 pdir = Vector3(dir.x_, 0, dir.z_);

		botAi_->node_->SetWorldDirection(pdir);
		Vector3 botVel = botAi_->rigidbody_->GetLinearVelocity();
		float botSpeed = botVel.Length();
		if (botSpeed < botAi_->SPEED + (botAi_->SPEED / 2.0f)) botAi_->rigidbody_->ApplyForce(dir * botAi_->SPEED);
	}
	else 
	{
		botAi_->selectedCheckPoint = NULL;
		botAi_->rigidbody_->SetLinearVelocity(Vector3::ZERO);
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("CheckPoint", eventData);
	}
}

void BotAIStateCheckPoint::EnterState()
{
	BotAIState::EnterState();
	botAi_->sfxRunSource_->Play(botAi_->gameWorld_->sfx.botRunSound);
}

void BotAIStateCheckPoint::ExitState()
{
	BotAIState::ExitState();
	botAi_->sfxRunSource_->Stop();
}


///
/// FIRE STATE
///
void BotAIStateFire::Update(float timeStep)
{
	botAi_->animFire_->SetWeight(1.0f);
	botAi_->animFire_->AddTime(timeStep);

	float t = botAi_->animFire_->GetTime();
	float l = botAi_->animFire_->GetLength() - timeStep;

	if (t >= l)
	{
		botAi_->animFire_->SetTime(0.0f); // restart anim
	}

}

void BotAIStateFire::FixedUpdate(float timeStep)
{
	Vector3 playerPos = botAi_->target_->GetWorldPosition();
	Vector3 botPos = botAi_->node_->GetWorldPosition();
	float distanceBetween = (playerPos - botPos).Length();
	
	if (distanceBetween < botAi_->ATTACKRANGE) // player in range, go fire
	{
		Vector3 dir = (playerPos - botPos);
		Vector3 pdir = Vector3(dir.x_, 0, dir.z_); // direction on plane
		botAi_->node_->SetWorldDirection(pdir);
		botAi_->boneCenter_->node_->SetWorldDirection(-dir);

	}
	else // lost player
	{
		botAi_->target_ = NULL;
		botAi_->ChangeState(botAi_->states[BotAIState::ANI_IDLE_1]);
	}

}

void BotAIStateFire::EnterState() 
{
	BotAIState::EnterState();
	botAi_->boneCenter_->animated_ = false;

}

void BotAIStateFire::ExitState() 
{
	BotAIState::ExitState();
	botAi_->boneCenter_->animated_ = true;
}


///
/// FALLING STATE
///
void BotAIStateFalling::Update(float timeStep)
{
	botAi_->animFalling_->SetWeight(1.0f);
	botAi_->animFalling_->AddTime(timeStep);

	float t = botAi_->animFalling_->GetTime();
	float l = botAi_->animFalling_->GetLength() - timeStep;

	if (t >= l)
	{
		
		VariantMap eventData;
		eventData["BOT"] = botAi_;
		botAi_->SendEvent("BotDestroy", eventData);
	}
}

void BotAIStateFalling::FixedUpdate(float timeStep)
{

}