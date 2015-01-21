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
						ANI_HITED_UP,
						ANI_ATTACK_WITH_GUN, ANI_ATTACK_WITH_CLAW,
						ANI_LAST };

	//enum AIStates { STATE_NONE = 0, STATE_IDLE, STATE_CHECKPOINT, STATE_FIRE, STATE_FALLING, STATE_LAST };
	
	//AIStates stateType_;
	ANIMStates stateAni_;

	float playedTime_;
	int playedCount_;

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

class BigBotAIStateAttackWithClaw : public BigBotAIState
{

public:
	BigBotAIStateAttackWithClaw(){};
	BigBotAIStateAttackWithClaw(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_ATTACK_WITH_CLAW;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};

class BigBotAIStateHited : public BigBotAIState
{

public:
	BigBotAIStateHited(){};
	BigBotAIStateHited(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_HITED;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};


class BigBotAIStateHitedIdle : public BigBotAIState
{

public:
	BigBotAIStateHitedIdle(){};
	BigBotAIStateHitedIdle(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_HITED_IDLE;
		SetActor(bot);
		playedCount_ = 0;
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};

class BigBotAIStateHitedUp : public BigBotAIState
{

public:
	BigBotAIStateHitedUp(){};
	BigBotAIStateHitedUp(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_HITED_UP;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};


class BigBotAIStateSkill1A : public BigBotAIState
{

public:
	BigBotAIStateSkill1A(){};
	BigBotAIStateSkill1A(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_SKILL1A;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};

class BigBotAIStateSkill1B : public BigBotAIState
{

public:
	BigBotAIStateSkill1B(){};
	BigBotAIStateSkill1B(BigBot* bot)
	{
		stateAni_ = ANIMStates::ANI_SKILL1B;
		SetActor(bot);
	};

	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	virtual void EnterState();
	virtual void ExitState();
};
