#include "Common.h"
#include "BigBotStates.h"
//#include "BigBot.h"


BigBotAIState::BigBotAIState() 
{
	// basic init
}

BigBotAIState::BigBotAIState(BigBot* bot) 
{
	BigBot_ = bot;
}

void BigBotAIState::SetActor(BigBot* bot) 
{
	BigBot_ = bot;
}

void BigBotAIState::EnterState()
{
	// do something before state start	
	playedTime_ = 0.0f;
	BigBot_->animState_ = stateAni_;
}

void BigBotAIState::ExitState() 
{

	// do something on exit
	BigBot_->animPrevState_ = stateAni_;
}

void BigBotAIState::Update(float timeStep) 
{
	// update by frame
};

void BigBotAIState::FixedUpdate(float timeStep) 
{
	// update by physics time
};


////////////////////////////////////////////////////////////////////////// <<IDLE
void BigBotAIStateIdle::Update(float timeStep) 
{
	BigBot_->animIdle_->SetWeight(1.0f);
	BigBot_->animIdle_->AddTime(timeStep);

	float t = BigBot_->animIdle_->GetTime();
	float l = BigBot_->animIdle_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BIGBOT"] = BigBot_;
		BigBot_->SendEvent("GoToNewIdleState", eventData);
	}
}
void BigBotAIStateIdle::FixedUpdate(float timeStep) 
{

}

////////////////////////////////////////////////////////////////////////// < LOOK LEFT

void BigBotAIStateIdleLookLeft::Update(float timeStep)
{
	BigBot_->animLookLeft_->SetWeight(1.0f);
	BigBot_->animLookLeft_->AddTime(timeStep);

	float t = BigBot_->animLookLeft_->GetTime();
	float l = BigBot_->animLookLeft_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BIGBOT"] = BigBot_;
		BigBot_->SendEvent("GoToNewIdleState", eventData);
	}
}

void BigBotAIStateIdleLookLeft::FixedUpdate(float timeStep)
{
	
}

////////////////////////////////////////////////////////////////////////// < LOOK RIGHT

void BigBotAIStateIdleLookRight::Update(float timeStep)
{
	BigBot_->animLookRight_->SetWeight(1.0f);
	BigBot_->animLookRight_->AddTime(timeStep);

	float t = BigBot_->animLookRight_->GetTime();
	float l = BigBot_->animLookRight_->GetLength() - 0.05f;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BIGBOT"] = BigBot_;
		BigBot_->SendEvent("GoToNewIdleState", eventData);
	}
}

void BigBotAIStateIdleLookRight::FixedUpdate(float timeStep)
{

}


////////////////////////////////////////////////////////////////////////// < TURN LEFT

void BigBotAIStateIdleTurnLeft::Update(float timeStep) 
{
	
	BigBot_->animTurnLeft_->SetWeight(1.0f);
	BigBot_->animTurnLeft_->AddTime(timeStep);

	float t = BigBot_->animTurnLeft_->GetTime();
	float l = BigBot_->animTurnLeft_->GetLength() - 0.05f;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BIGBOT"] = BigBot_;
		BigBot_->SendEvent("GoToNewIdleState", eventData);

	}
}

void BigBotAIStateIdleTurnLeft::FixedUpdate(float timeStep)
{

}

void BigBotAIStateIdleTurnLeft::EnterState() 
{
	BigBotAIState::EnterState();
}

void BigBotAIStateIdleTurnLeft::ExitState() 
{
	BigBotAIState::ExitState();

	Quaternion q = BigBot_->rigidbody_->GetRotation();
	Quaternion n;
	//Vector3 euler = q.EulerAngles();
	n.FromEulerAngles(0.0f, -90.0f, 0.0f);
	//Vector3 dir = q * Vector3::LEFT;
	BigBot_->rigidbody_->SetRotation(n*q);

}

////////////////////////////////////////////////////////////////////////// < TURN RIGHT

void BigBotAIStateIdleTurnRight::Update(float timeStep)
{
	BigBot_->animTurnRight_->SetWeight(1.0f);
	BigBot_->animTurnRight_->AddTime(timeStep);

	float t = BigBot_->animTurnRight_->GetTime();
	float l = BigBot_->animTurnRight_->GetLength() - timeStep;

	if (t >= l)
	{
		VariantMap eventData;
		eventData["BIGBOT"] = BigBot_;
		BigBot_->SendEvent("GoToNewIdleState", eventData);

		
	}
}

void BigBotAIStateIdleTurnRight::FixedUpdate(float timeStep)
{

}

void BigBotAIStateIdleTurnRight::EnterState()
{
	BigBotAIState::EnterState();
}

void BigBotAIStateIdleTurnRight::ExitState()
{
	BigBotAIState::ExitState();

	Quaternion q = BigBot_->rigidbody_->GetRotation();
	Quaternion n;
	//Vector3 euler = q.EulerAngles();
	n.FromEulerAngles(0.0f, 90.0f, 0.0f);
	//Vector3 dir = q * Vector3::LEFT;
	BigBot_->rigidbody_->SetRotation(n*q);
	

}

////////////////////////////////////////////////////////////////////////// < ATTACK WITH GUN
void BigBotAIStateAttackWithGun::Update(float timeStep) 
{
	
	BigBot_->animAttackWithGun_->SetWeight(1.0f);
	BigBot_->animAttackWithGun_->AddTime(timeStep);

	float t = BigBot_->animAttackWithGun_->GetTime();
	float l = BigBot_->animAttackWithGun_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_IDLE]);
	}
}

void BigBotAIStateAttackWithGun::FixedUpdate(float timeStep)
{
	if (BigBot_->target == NULL) return;
	
	Vector3 playerPos = BigBot_->target->GetWorldPosition();
	Vector3 botPos = BigBot_->node_->GetWorldPosition();
	float distanceBetween = (playerPos - botPos).Length();

	if (distanceBetween < BigBot_->ATTACKRANGE) // player in range, go fire
	{
		// direction for non-influent on animation bone(Rotator) with fix by Y
		Vector3 dir = ((Vector3(0.0f,-5.0f,0.0f)) + (playerPos - botPos)).Normalized(); 
		// direction on plane for node
		Vector3 pdir = Vector3(dir.x_, 0.0f, dir.z_);
		// rotate Node for looking on player
		BigBot_->node_->SetWorldDirection(pdir);
		// rotate Bone-Rotator to Player 
		BigBot_->boneCenter_->node_->SetWorldDirection( -dir);

	}
	else // lost player
	{
	
		BigBot_->target = NULL; // reset target
	}
}

void BigBotAIStateAttackWithGun::EnterState()
{
	// call std enter for all animation stages
	BigBotAIState::EnterState();
	// when this animation state going we off the bone from animation for manual rotate
	BigBot_->boneCenter_->animated_ = false;
}

void BigBotAIStateAttackWithGun::ExitState()
{
	// call std exit for all animation stages
	BigBotAIState::ExitState();
	// when state ends we on bone to animation 
	BigBot_->boneCenter_->animated_ = true;
}
////////////////////////////////////////////////////////////////////////// < ATTACK CLAW
void BigBotAIStateAttackWithClaw::Update(float timeStep) 
{
	BigBot_->animAttackWithClaw_->SetWeight(1.0f);
	BigBot_->animAttackWithClaw_->AddTime(timeStep);

	float t = BigBot_->animAttackWithClaw_->GetTime();
	float l = BigBot_->animAttackWithClaw_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_IDLE]);
	}
}

void BigBotAIStateAttackWithClaw::FixedUpdate(float timeStep)
{
	if (BigBot_->target == NULL) return;

	Vector3 playerPos = BigBot_->target->GetWorldPosition();
	Vector3 botPos = BigBot_->node_->GetWorldPosition();
	float distanceBetween = (playerPos - botPos).Length();

	if (distanceBetween < BigBot_->ATTACKRANGE_LOW) // player in range, go fire
	{
		// direction for non-influent on animation bone(Rotator) with fix by Y
		Vector3 dir = ((Vector3(0.0f, -4.0f, 0.0f)) + (playerPos - botPos)).Normalized();
		// direction on plane for node
		Vector3 pdir = Vector3(dir.x_, 0.0f, dir.z_);
		// rotate Node for looking on player
		BigBot_->node_->SetWorldDirection(pdir);
		// rotate Bone-Rotator to Player 
		BigBot_->boneCenter_->node_->SetWorldDirection(-dir);

	}
	else // lost player
	{

		BigBot_->target = NULL; // reset target
	}
}

void BigBotAIStateAttackWithClaw::EnterState()
{
	BigBotAIState::EnterState();
	BigBot_->boneCenter_->animated_ = false;
}

void BigBotAIStateAttackWithClaw::ExitState()
{
	BigBotAIState::ExitState();
	BigBot_->boneCenter_->animated_ = true;
}


////////////////////////////////////////////////////////////////////////// < HITED
void BigBotAIStateHited::Update(float timeStep) 
{
	BigBot_->animHited_->SetWeight(1.0f);
	BigBot_->animHited_->AddTime(timeStep);

	float t = BigBot_->animHited_->GetTime();
	float l = BigBot_->animHited_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_HITED_IDLE]);
	}
}

void BigBotAIStateHited::FixedUpdate(float timeStep) 
{
	
}

void BigBotAIStateHited::EnterState() 
{
	BigBotAIState::EnterState();
}

void BigBotAIStateHited::ExitState() 
{
	BigBotAIState::ExitState();
}

////////////////////////////////////////////////////////////////////////// < HITED IDLE

void BigBotAIStateHitedIdle::Update(float timeStep)
{
	BigBot_->animHitedIdle_->SetWeight(1.0f);
	BigBot_->animHitedIdle_->AddTime(timeStep);

	float t = BigBot_->animHitedIdle_->GetTime();
	float l = BigBot_->animHitedIdle_->GetLength() - timeStep;

	if (t >= l)
	{
		if (playedCount_ < 10) {
			playedCount_++;
			//BigBot_->animHitedIdle_->AddTime(0.0f);
		}
		else 
		{
			BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_HITED_UP]);
		}
	}
}

void BigBotAIStateHitedIdle::FixedUpdate(float timeStep)
{

}

void BigBotAIStateHitedIdle::EnterState()
{
	BigBotAIState::EnterState();
	playedCount_ = 0;
}

void BigBotAIStateHitedIdle::ExitState()
{
	BigBotAIState::ExitState();
	playedCount_ = 0;
}

////////////////////////////////////////////////////////////////////////// < HITED UP

void BigBotAIStateHitedUp::Update(float timeStep)
{
	BigBot_->animHitedUp_->SetWeight(1.0f);
	BigBot_->animHitedUp_->AddTime(timeStep);

	float t = BigBot_->animHitedUp_->GetTime();
	float l = BigBot_->animHitedUp_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_IDLE]);
	}
}

void BigBotAIStateHitedUp::FixedUpdate(float timeStep)
{

}

void BigBotAIStateHitedUp::EnterState()
{
	BigBotAIState::EnterState();
}

void BigBotAIStateHitedUp::ExitState()
{
	BigBotAIState::ExitState();
}

////////////////////////////////////////////////////////////////////////// < SKILL1_A

void BigBotAIStateSkill1A::Update(float timeStep) 
{
	BigBot_->animSkill1A_->SetWeight(1.0f);
	BigBot_->animSkill1A_->AddTime(timeStep);

	float t = BigBot_->animSkill1A_->GetTime();
	float l = BigBot_->animSkill1A_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_SKILL1B]);
	}
}

void BigBotAIStateSkill1A::FixedUpdate(float timeStep)
{

}

void BigBotAIStateSkill1A::EnterState()
{
	BigBotAIState::EnterState();
}

void BigBotAIStateSkill1A::ExitState()
{
	BigBotAIState::ExitState();
}

////////////////////////////////////////////////////////////////////////// < SKILL1_B

void BigBotAIStateSkill1B::Update(float timeStep)
{
	BigBot_->animSkill1B_->SetWeight(1.0f);
	BigBot_->animSkill1B_->AddTime(timeStep);

	float t = BigBot_->animSkill1B_->GetTime();
	float l = BigBot_->animSkill1B_->GetLength() - timeStep;

	if (t >= l)
	{
		BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_IDLE]);
	}
}

void BigBotAIStateSkill1B::FixedUpdate(float timeStep)
{

}

void BigBotAIStateSkill1B::EnterState()
{
	BigBotAIState::EnterState();
}

void BigBotAIStateSkill1B::ExitState()
{
	BigBotAIState::ExitState();
}