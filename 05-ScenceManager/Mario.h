#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED		0.15f 
//0.1f
#define MARIO_JUMP_SPEED_Y		0.25f
#define MARIO_JUMP_DEFLECT_SPEED 0.2f
#define MARIO_GRAVITY			0.002f
#define JUMP_GRAVITY			0.0005f
#define MARIO_DIE_DEFLECT_SPEED	 0.5f
#define MARIO_JUMP_WHILE_WALK_SPEED_Y		0.125f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_DIE				400
#define MARIO_STATE_ATTACK_UP_RIGHT	500
#define MARIO_STATE_STAND_ATTACK_RIGHT	600
#define MARIO_STATE_WALK_UP_RIGHT	700
#define MARIO_STATE_JUMP_UP	800
#define MARIO_STATE_ATTACK_UP_LEFT	500

#define MARIO_ANI_BIG_IDLE_RIGHT		0
#define MARIO_ANI_BIG_IDLE_LEFT			1
#define MARIO_ANI_SMALL_IDLE_RIGHT		2
#define MARIO_ANI_SMALL_IDLE_LEFT			3

#define MARIO_ANI_BIG_WALKING_RIGHT			4
#define MARIO_ANI_BIG_WALKING_LEFT			5
#define MARIO_ANI_SMALL_WALKING_RIGHT		6
#define MARIO_ANI_SMALL_WALKING_LEFT		7

#define MARIO_ANI_DIE				8
#define MARIO_ANI_JUMP_RIGHT				9
#define MARIO_ANI_JUMP_LEFT				10
#define MARIO_ANI_ATTACK_UP_RIGHT				11
#define MARIO_ANI_STAND_ATTACK_RIGHT				12
#define MARIO_ANI_WALK_UP_RIGHT				13
#define MARIO_ANI_JUMP_UP_RIGHT				14
#define MARIO_ANI_ATTACK_UP_LEFT				15

#define	MARIO_LEVEL_SMALL			1
#define	MARIO_LEVEL_BIG				2
#define	MARIO_LEVEL_ATTACK_UP		3

#define MARIO_BIG_BBOX_WIDTH  15
#define MARIO_BIG_BBOX_HEIGHT 20

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_ATTACK_UP_BBOX_WIDTH  13
#define MARIO_ATTACK_UP_BBOX_HEIGHT 50

#define MARIO_UNTOUCHABLE_TIME 5000
#define MARIO_JUMP_TIME 1000


class CMario : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y; 
public: 
	boolean isJumping, isAttackUp, isStandAttack, isJumpingWhileWalk;
	DWORD jump_start, attack_start;
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
    void fire(vector<LPGAMEOBJECT> &objects);
	void Reset();
	void ResetJump();
	void ResetAttackUp();
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};