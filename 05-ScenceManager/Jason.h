#pragma once
#include "GameObject.h"

#define JASON_WALKING_SPEED		0.15f 
//0.1f
#define JASON_JUMP_SPEED_Y		0.25f
#define JASON_JUMP_DEFLECT_SPEED 0.2f
#define JASON_GRAVITY			0.002f
#define JUMP_GRAVITY			0.0005f
#define JASON_DIE_DEFLECT_SPEED	 0.5f

#define JASON_STATE_IDLE			0
#define JASON_STATE_WALKING_RIGHT	100
#define JASON_STATE_WALKING_LEFT	200
#define JASON_STATE_JUMP			300
#define JASON_STATE_DIE				400
#define JASON_STATE_ATTACK_UP_RIGHT	500
#define JASON_STATE_STAND_ATTACK_RIGHT	600
#define JASON_STATE_WALK_UP_RIGHT	700
#define JASON_STATE_JUMP_UP	800
#define JASON_STATE_ATTACK_UP_LEFT	500

#define JASON_ANI_IDLE_RIGHT		0
#define JASON_ANI_IDLE_LEFT			1
#define JASON_ANI_SMALL_IDLE_RIGHT		2
#define JASON_ANI_SMALL_IDLE_LEFT			3

#define JASON_ANI_WALKING_RIGHT			4
#define JASON_ANI_WALKING_LEFT			5
#define JASON_ANI_SMALL_WALKING_RIGHT		6
#define JASON_ANI_SMALL_WALKING_LEFT		7

#define JASON_ANI_DIE				8
#define JASON_ANI_JUMP_RIGHT				9
#define JASON_ANI_JUMP_LEFT				10
#define JASON_ANI_ATTACK_UP_RIGHT				11
#define JASON_ANI_STAND_ATTACK_RIGHT				12
#define JASON_ANI_WALK_UP_RIGHT				13
#define JASON_ANI_JUMP_UP_RIGHT				14
#define JASON_ANI_ATTACK_UP_LEFT				15

#define	JASON_LEVEL_SMALL			1
#define	JASON_LEVEL_BIG				2
#define	JASON_LEVEL_ATTACK_UP		3

//#define JASON_BIG_BBOX_WIDTH  15
//#define JASON_BIG_BBOX_HEIGHT 27
#define JASON_BIG_BBOX_WIDTH  5
#define JASON_BIG_BBOX_HEIGHT 3

#define JASON_SMALL_BBOX_WIDTH  13
#define JASON_SMALL_BBOX_HEIGHT 15

#define JASON_ATTACK_UP_BBOX_WIDTH  13
#define JASON_ATTACK_UP_BBOX_HEIGHT 50

#define JASON_UNTOUCHABLE_TIME 5000
#define JASON_JUMP_TIME 1000


class CJason : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Jason at scene
	float start_y; 
public: 
	boolean isJumping, isAttackUp, isStandAttack;
	DWORD jump_start, attack_start;
	CJason(float x = 0.0f, float y = 0.0f);
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