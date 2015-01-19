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
		//VariantMap eventData;
		//eventData["BIGBOT"] = BigBot_;
		//BigBot_->SendEvent("GoToNewIdleState", eventData);

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
		Vector3 dir = ((Vector3(0.0f,-5.0f,0.0f)) + (playerPos - botPos)).Normalized();
		Quaternion q;
		//q.FromRotationTo(playerPos.Normalized(), botPos.Normalized());
		
		Vector3 pdir = Vector3(dir.x_, 0.0f, dir.z_); // direction on plane
		//pdir.Normalized();
		//BigBot_->node_->SetWorldRotation(q);
		BigBot_->node_->SetWorldDirection(pdir.CrossProduct(Vector3::UP));
		BigBot_->boneCenter_->node_->SetWorldDirection( -dir);

	}
	else // lost player
	{
	
		BigBot_->target = NULL;
		//BigBot_->ChangeState(BigBot_->states[BigBotAIState::ANI_IDLE]);
	}
}

void BigBotAIStateAttackWithGun::EnterState()
{

	BigBotAIState::EnterState();
	BigBot_->boneCenter_->animated_ = false;
}

void BigBotAIStateAttackWithGun::ExitState()
{
	BigBotAIState::ExitState();
	BigBot_->boneCenter_->animated_ = true;
}