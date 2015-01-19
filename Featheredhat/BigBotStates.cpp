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