#pragma once

class BigBot;

class BigBotAIState
{
protected:
	BigBot* BigBot_;

public:
	enum ANIMStates {	ANI_NONE = 0, 
						ANI_IDLE, 
						ANI_IDLE_LOOK_LEFT, 
						ANI_IDLE_LOOK_RIGHT, 
						ANI_TURN_LEFT, 
						ANI_TURN_RIGHT, 
						ANI_SKILL1A, ANI_SKILL1B, 
						ANI_WALK, 
						ANI_HITED,
						ANI_HITED_IDLE,
						ANI_ATTACK_WITH_GUN,
						ANI_LAST };

	//enum AIStates { STATE_NONE = 0, STATE_IDLE, STATE_CHECKPOINT, STATE_FIRE, STATE_FALLING, STATE_LAST };
	
	//AIStates stateType_;
	ANIMStates stateAni_;

	float playedTime_;

	BigBotAIState();
	BigBotAIState(BigBot* bot);
	void SetActor(BigBot* bot);
	virtual void EnterState();
	virtual void ExitState();

	virtual void Update(float timeStep);
	virtual void FixedUpdate(float timeStep);
	virtual ~BigBotAIState() { BigBot_ = NULL; };

};

class BigBotAIStateIdle : public BigBotAIState
{

public:
	BigBotAIStateIdle(){};
	BigBotAIStateIdle(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_IDLE;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};

class BigBotAIStateIdleLookLeft : public BigBotAIState
{

public:
	BigBotAIStateIdleLookLeft(){};
	BigBotAIStateIdleLookLeft(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_IDLE_LOOK_LEFT;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};

class BigBotAIStateIdleLookRight : public BigBotAIState
{

public:
	BigBotAIStateIdleLookRight(){};
	BigBotAIStateIdleLookRight(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_IDLE_LOOK_RIGHT;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
};

class BigBotAIStateIdleTurnLeft : public BigBotAIState
{

public:
	BigBotAIStateIdleTurnLeft(){};
	BigBotAIStateIdleTurnLeft(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_TURN_LEFT;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};

class BigBotAIStateIdleTurnRight : public BigBotAIState
{

public:
	BigBotAIStateIdleTurnRight(){};
	BigBotAIStateIdleTurnRight(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_TURN_RIGHT;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};

class BigBotAIStateAttackWithGun : public BigBotAIState
{

public:
	BigBotAIStateAttackWithGun(){};
	BigBotAIStateAttackWithGun(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_ATTACK_WITH_GUN;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};