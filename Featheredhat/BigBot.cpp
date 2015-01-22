#include "Common.h"
#include "BigBot.h"

BigBot::BigBot(Context* context) : LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);
}

void BigBot::RegisterObject(Context* context)
{
	context->RegisterFactory<BigBot>();
}

void BigBot::SetGameWorld(struct GameWorld& gameWorld)
{
	gameWorld_ = &gameWorld;
}

void BigBot::Stop()
{
	gameWorld_ = NULL;
	state_ = NULL;
	if (states[BigBotAIState::ANI_IDLE]) delete states[BigBotAIState::ANI_IDLE];
	if (states[BigBotAIState::ANI_IDLE_LOOK_LEFT]) delete states[BigBotAIState::ANI_IDLE_LOOK_LEFT];
	if (states[BigBotAIState::ANI_IDLE_LOOK_RIGHT]) delete states[BigBotAIState::ANI_IDLE_LOOK_RIGHT];

}

void BigBot::Start()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	// get helpers
	node_ = GetNode()->GetChild("BIGBOT", true);
	//rayNode_ = node_->GetChild("rayNode", true);
	//fireNode_ = node_->GetChild("FireJoint", true);
	
	footL_ = node_->GetChild("footL", true);
	footR_ = node_->GetChild("footR", true);

	// get components
	rigidbody_ = node_->GetComponent<RigidBody>();
	animModel_ = node_->GetComponent<AnimatedModel>();
	animController_ = node_->GetComponent<AnimationController>();
	boneCenter_ = animModel_->GetSkeleton().GetBone("Rotator");
	//boneCenter_->animated_ = false;

	laserJoint = node_->GetChild("LaserJoint", true);
	laserNode = laserJoint->GetChild("LaserFx", true);
	laserModel = laserNode->GetComponent<AnimatedModel>();
	laserAnimState = laserModel->GetAnimationState(StringHash("LASERFIRE"));
	laserAnimState->SetLooped(false);
	laserAnimState->SetWeight(1.0f);
	laserAnimState->SetLayer(1);
	
	laserNode->SetEnabled(false);
	

	// get animations
	animIdle_ = animModel_->GetAnimationState(StringHash("BIGBOT_IDLE"));
	animTurnLeft_ = animModel_->GetAnimationState(StringHash("BIGBOT_TURN_LEFT"));
	animTurnRight_ = animModel_->GetAnimationState(StringHash("BIGBOT_TURN_RIGHT"));
	animLookLeft_ = animModel_->GetAnimationState(StringHash("BIGBOT_IDLE_LOOK_LEFT"));
	animLookRight_ = animModel_->GetAnimationState(StringHash("BIGBOT_IDLE_LOOK_RIGHT"));
	animSkill1A_ = animModel_->GetAnimationState(StringHash("BIGBOT_SKILL_1A"));
	animSkill1B_ = animModel_->GetAnimationState(StringHash("BIGBOT_SKILL_1B"));
	animHited_ = animModel_->GetAnimationState(StringHash("BIGBOT_HITED"));
	animHitedIdle_ = animModel_->GetAnimationState(StringHash("BIGBOT_HITED_IDLE"));
	animWalk_ = animModel_->GetAnimationState(StringHash("BIGBOT_WALK"));
	animAttackWithGun_ = animModel_->GetAnimationState(StringHash("BIGBOT_ATTACK_1"));
	animAttackWithClaw_ = animModel_->GetAnimationState(StringHash("BIGBOT_ATTACK_2"));
	animHitedUp_ = animModel_->GetAnimationState(StringHash("BIGBOT_HITED_UP"));



	animIdle_->SetLooped(true);
	animTurnLeft_->SetLooped(false);
	animTurnRight_->SetLooped(false);
	animLookLeft_->SetLooped(false);
	animLookRight_->SetLooped(false);
	animSkill1A_->SetLooped(false);
	animSkill1B_->SetLooped(false);
	animHited_->SetLooped(false);
	animHitedIdle_->SetLooped(true);
	animWalk_->SetLooped(true);
	animAttackWithGun_->SetLooped(false);
	animAttackWithClaw_->SetLooped(false);
	animHitedUp_->SetLooped(false);

	animIdle_->SetWeight(1.0f);
	animTurnLeft_->SetWeight(0.0f);
	animTurnRight_->SetWeight(0.0f);
	animLookLeft_->SetWeight(0.0f);
	animLookRight_->SetWeight(0.0f);
	animSkill1A_->SetWeight(0.0f);
	animSkill1B_->SetWeight(0.0f);
	animHited_->SetWeight(0.0f);
	animHitedIdle_->SetWeight(0.0f);
	animWalk_->SetWeight(0.0f);
	animAttackWithGun_->SetWeight(0.0f);
	animAttackWithClaw_->SetWeight(0.0f);
	animHitedUp_->SetWeight(0.0f);

	animIdle_->SetLayer(1);
	animTurnLeft_->SetLayer(2);
	animTurnRight_->SetLayer(2);
	animLookLeft_->SetLayer(3);
	animLookRight_->SetLayer(3);
	animSkill1A_->SetLayer(4);
	animSkill1B_->SetLayer(4);
	animHited_->SetLayer(5);
	animHitedIdle_->SetLayer(5);
	animWalk_->SetLayer(6);
	animAttackWithGun_->SetLayer(7);
	animAttackWithClaw_->SetLayer(7);
	animHitedUp_->SetLayer(5);

	
	SubscribeToEvent(node_, E_ANIMATIONTRIGGER, HANDLER(BigBot, HandleAnimationTrigger));
	SubscribeToEvent("GoToNewIdleState", HANDLER(BigBot, HandleNewIdleState));
	SubscribeToEvent("BotDestroy", HANDLER(BigBot, HandleBotDestroy));
	SubscribeToEvent("CheckPoint", HANDLER(BigBot, HandleCheckPoint));
	SubscribeToEvent(StringHash("BigBotYouAreHit"), HANDLER(BigBot, HandleHit));


	// delo truba
	helperTruba1_ = node_->GetChild("Truba1", true);
	helperTruba2_ = node_->GetChild("Truba2", true);
	helperTruba3_ = node_->GetChild("Truba3", true);
	cur_stage = 0;
	stage = 0;
	stage_time = 0.0f;

	// hited smokes
	helperSmoke[0] = node_->GetChild("smoke1", true);
	helperSmoke[1] = node_->GetChild("smoke2", true);
	helperSmoke[2] = node_->GetChild("smoke3", true);
	helperSmoke[3] = node_->GetChild("smoke4", true);
	helperSmoke[4] = node_->GetChild("smoke5", true);

	states[BigBotAIState::ANI_IDLE] = new BigBotAIStateIdle(this);
	states[BigBotAIState::ANI_IDLE_LOOK_LEFT] = new BigBotAIStateIdleLookLeft(this);
	states[BigBotAIState::ANI_IDLE_LOOK_RIGHT] = new BigBotAIStateIdleLookRight(this);
	states[BigBotAIState::ANI_TURN_LEFT] = new BigBotAIStateIdleTurnLeft(this);
	states[BigBotAIState::ANI_TURN_RIGHT] = new BigBotAIStateIdleTurnRight(this);
	states[BigBotAIState::ANI_ATTACK_WITH_GUN] = new BigBotAIStateAttackWithGun(this);
	states[BigBotAIState::ANI_HITED] = new BigBotAIStateHited(this);
	states[BigBotAIState::ANI_HITED_IDLE] = new BigBotAIStateHitedIdle(this);
	states[BigBotAIState::ANI_ATTACK_WITH_CLAW] = new BigBotAIStateAttackWithClaw(this);
	states[BigBotAIState::ANI_HITED_UP] = new BigBotAIStateHitedUp(this);
	states[BigBotAIState::ANI_SKILL1A] = new BigBotAIStateSkill1A(this);
	states[BigBotAIState::ANI_SKILL1B] = new BigBotAIStateSkill1B(this);
	

	animPrevState_ = BigBotAIState::ANI_NONE;
	state_ = NULL;
	// bigbot starting state
	ChangeState(states[BigBotAIState::ANI_IDLE]);

	// unparent from BBPrefab
	GetNode()->SetParent(GetScene());
}

void BigBot::HandleAnimationTrigger(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;

	AnimationState* as = animModel_->GetAnimationState(StringHash(eventData[P_NAME].GetString()));
	// Анимация ходьбы, истансы пыли из под ног
	if (as == animWalk_) 
	{
		Vector3 pos;
		String value = eventData[P_DATA].GetString();
		if (value == "StepL")
			pos = footL_->GetWorldPosition();
		else
			pos = footR_->GetWorldPosition();

		Quaternion q = footL_->GetWorldRotation();

		Node* smokeNode = GetScene()->InstantiateXML(gameWorld_->prefabs.prefabSmokeFx_->GetRoot(), pos, q, LOCAL);
		smokeNode->SetWorldScale(Vector3::ONE* 0.5f);
		ScriptSmokeFx* script = smokeNode->CreateComponent<ScriptSmokeFx>();
		//script->SetOrientationToNode(gameWorld_->camera.TPCLogic_->cameraNode_);
		
	}
	else if (as == animTurnLeft_ || as == animTurnRight_ || as == animAttackWithGun_)
	{
		Vector3 pos;
		String value = eventData[P_DATA].GetString();
		if (value == "laser") 
		{
			laserNode->SetParent(GetScene());
		}
		else 
		{
			if (value == "StepL")
				pos = footL_->GetWorldPosition();
			else
				pos = footR_->GetWorldPosition();

			Quaternion q = footL_->GetWorldRotation();

			Node* smokeNode = GetScene()->InstantiateXML(gameWorld_->prefabs.prefabSmokeFx_->GetRoot(), pos, q, LOCAL);
			smokeNode->SetWorldScale(Vector3::ONE* 0.5f);
			ScriptSmokeFx* script = smokeNode->CreateComponent<ScriptSmokeFx>();
		}
	}
	else if (as == animHited_) 
	{
		String value = eventData[P_DATA].GetString();
		if (value == "smoke")
		{
			for (unsigned int i = 0; i < 5; i++)
			{
				Quaternion q = helperSmoke[i]->GetWorldRotation();
				Vector3 p = helperSmoke[i]->GetWorldPosition();
				Node* smokeNode = GetScene()->InstantiateXML(gameWorld_->prefabs.prefabSmokeFx_->GetRoot(), p, q, LOCAL);
				//smokeNode->SetWorldScale(Vector3::ONE* 0.5f);
				ScriptSmokeFx* script = smokeNode->CreateComponent<ScriptSmokeFx>();
				
			}

			// бот упал... полетели брызги... кочнулсь камера...
			VariantMap& eventData = GetNode()->GetEventDataMap();
			eventData[P_DATA] = GetNode()->GetWorldPosition();
			SendEvent(StringHash("Blast"), eventData);

		}
		else if (value == "laser") 
		{
			//laserNode->SetEnabled(true);
			//laserNode->SetParent(GetScene());
			
		}
	}

}

void BigBot::HandleNewIdleState(StringHash eventType, VariantMap& eventData)
{
	if (state_)
	if (eventData["BIGBOT"].GetPtr() == this)
	//if (state_->stateType_ == BotAIState::STATE_IDLE)
	{

		BigBotAIState::ANIMStates r = (BigBotAIState::ANIMStates) Random((int)BigBotAIState::ANI_IDLE, (int)BigBotAIState::ANI_TURN_RIGHT);
		ChangeState(states[r]);
	}

}

void BigBot::HandleBotDestroy(StringHash eventType, VariantMap& eventData)
{

}

void BigBot::HandleCheckPoint(StringHash eventType, VariantMap& eventData) 
{

}

void BigBot::HandleHit(StringHash eventType, VariantMap& eventData)
{
	using namespace AnimationTrigger;

	// нод который прислали
	Node* otherNode = (Node*)eventData[P_DATA].GetVoidPtr();
	
	if (otherNode == NULL) return;

	// найдем с таким же именем в моделе
	Node* childNode = GetNode()->GetChild(otherNode->GetName(), true);

	// если это один и тот же нод, значит сообщение нам 
	if (childNode == otherNode)
	if (animState_ != BigBotAIState::ANI_HITED)
	if (animState_ != BigBotAIState::ANI_HITED_IDLE)
	{
		// change to hit anim
		ChangeState(states[BigBotAIState::ANI_HITED]);
	}
}

void BigBot::Update(float timeStep)
{
	//animIdle_->AddTime(timeStep);
	if (state_) state_->Update(timeStep);
}

void BigBot::FixedUpdate(float timeStep)
{
	SmokeAnimation(timeStep);

	// если бот не подбит и не валяется пусть проверит наличие игрока рядом 
	if (animState_ != BigBotAIState::ANI_HITED)  
		if (animState_ != BigBotAIState::ANI_HITED_IDLE)
		if (animState_ != BigBotAIState::ANI_HITED_UP)
		if (animState_ != BigBotAIState::ANI_ATTACK_WITH_GUN)
		if (animState_ != BigBotAIState::ANI_ATTACK_WITH_CLAW)
		{
			CheckForFireByPlayer(ATTACKRANGE);
		}

	if (state_) state_->FixedUpdate(timeStep);

}


void BigBot::SmokeAnimation(float timeStep)
{
	const float betweenTime = 0.2f;

	if (stage == 0) 
	{
		// create instance
		//ScriptSmokeFx* smoke = GetScene()->InstantiateXML();
		Quaternion q;
		Vector3 pos;
		
		if (cur_stage == 0)
			pos = helperTruba1_->GetWorldPosition();
		else if (cur_stage == 1)
			pos = helperTruba2_->GetWorldPosition();
		else 
			pos = helperTruba3_->GetWorldPosition();

		q = helperTruba1_->GetWorldRotation();

		Node* smokeNode = GetScene()->InstantiateXML(gameWorld_->prefabs.prefabBlackSmokeFx_->GetRoot(), pos, q, LOCAL);
		smokeNode->SetWorldScale(Vector3::ONE * 0.3f);
		ScriptSmokeFx* script = smokeNode->CreateComponent<ScriptSmokeFx>();
		script->SetOrientationToNode(gameWorld_->camera.TPCLogic_->cameraNode_);
		//script->SetStartEndColors(Vector4(0.2f, 0.2f, 0.2f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));

		stage = 1;
		stage_time = 0.0f;
	}
	else
	{
		if (stage_time >= betweenTime) 
		{
			stage_time = 0.0f;
			stage = 0;
			// new stage
			if (cur_stage < 2) 
				cur_stage++; 
			else 
				cur_stage = 0;
		}
		else 
		{
			stage_time += timeStep;
			// old
		}
	}
}

void BigBot::ChangeState(BigBotAIState* newState) 
{
	if (state_)
	{
		state_->ExitState();
	}

	state_ = newState;
	state_->EnterState();
	
	ClearPrevAnimStates();
}

void BigBot::ClearPrevAnimStates() 
{
	if (animPrevState_ == BigBotAIState::ANI_NONE) return; // if nothing to erase 
	if (animPrevState_ == animState_) return;

	//turn-off previous anim state
	switch (animPrevState_)
	{
	case BigBotAIState::ANI_IDLE:
	{

								   //animIdle1_->SetWeight(0.0f); // this lower priority state it's always on
								   animIdle_->SetTime(0.0);
								   break;
	}
	case BigBotAIState::ANI_IDLE_LOOK_LEFT:
	{
								   animLookLeft_->SetWeight(0.0f);
								   animLookLeft_->SetTime(0.0);
								   break;
	}
	case BigBotAIState::ANI_IDLE_LOOK_RIGHT:
	{
								   animLookRight_->SetWeight(0.0f);
								   animLookRight_->SetTime(0.0);
								   break;
	}
	case BigBotAIState::ANI_TURN_LEFT:
	{
								   animTurnLeft_->SetWeight(0.0f);
								   animTurnLeft_->SetTime(0.0);
								   break;
	}
	case BigBotAIState::ANI_TURN_RIGHT:
	{
								   animTurnRight_->SetWeight(0.0f);
								   animTurnRight_->SetTime(0.0);
								   break;
	}
	case BigBotAIState::ANI_WALK:
	{
								 animWalk_->SetWeight(0.0f);
								 animWalk_->SetTime(0.0);
								 break;
	}
	case BigBotAIState::ANI_ATTACK_WITH_GUN:
	{
								 animAttackWithGun_->SetWeight(0.0f);
								 animAttackWithGun_->SetTime(0.0f);
								 laserAnimState->SetTime(0.0f);


								 break;
	}
	case BigBotAIState::ANI_HITED:
	{
									animHited_->SetWeight(0.0f);
									animHited_->SetTime(0.0f);
									break;
	}
	case BigBotAIState::ANI_HITED_IDLE:
	{
									 animHitedIdle_->SetWeight(0.0f);
									 animHitedIdle_->SetTime(0.0f);
									 break;
	}
	case BigBotAIState::ANI_SKILL1A:
	{
								animSkill1A_->SetWeight(0.0f);
								animSkill1A_->SetTime(0.0f);
								break;
	}
	case BigBotAIState::ANI_SKILL1B:
	{
		animSkill1B_->SetWeight(0.0f);
		animSkill1B_->SetTime(0.0f);
		break;
	}
	case BigBotAIState::ANI_ATTACK_WITH_CLAW:
	{
									   animAttackWithClaw_->SetWeight(0.0f);
									   animAttackWithClaw_->SetTime(0.0f);
									   break;
	}
	case BigBotAIState::ANI_HITED_UP:
	{
									   animHitedUp_->SetWeight(0.0f);
									   animHitedUp_->SetTime(0.0f);
									   break;
	}
	}

	animPrevState_ = BigBotAIState::ANI_NONE;
}


void BigBot::CheckForFireByPlayer(float range_)
{
	if (animState_ == BigBotAIState::ANI_ATTACK_WITH_GUN) return;
	if (animState_ == BigBotAIState::ANI_ATTACK_WITH_CLAW) return;


	Vector3 botPos = node_->GetWorldPosition();
	Vector3 playerPos = gameWorld_->player.node_->GetWorldPosition();
	float distanceBetween = (botPos - playerPos).Length();

	float distanceByY = (botPos.y_ - playerPos.y_);


	if (distanceBetween < range_ && distanceByY < 0.0f) // player in range, go fire
	{
		target = gameWorld_->player.node_;
		if (distanceBetween > ATTACKRANGE_LOW)
			ChangeState(states[BigBotAIState::ANI_ATTACK_WITH_GUN]);
		else
			ChangeState(states[BigBotAIState::ANI_ATTACK_WITH_CLAW]);


		isAttacking = true;
	}
	isAttacking = false;
}