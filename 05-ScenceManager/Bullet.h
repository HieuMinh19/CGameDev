#pragma once
#include "GameObject.h"

#define BULLET_WALKING_SPEED 0.25f;

#define BULLET_BBOX_WIDTH 5
#define BULLET_BBOX_HEIGHT 5

#define BULLET_STATE_FLYING 100
#define BULLET_STATE_DIE 200

class CBullet : public CGameObject
{
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

public:
	CBullet();
	virtual void SetState(int state);
};