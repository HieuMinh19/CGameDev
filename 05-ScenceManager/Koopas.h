#pragma once

#include "GameObject.h"

#define KOOPAS_WALKING_SPEED 0.03f;

#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DIE 16

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DIE -1
#define KOOPAS_STATE_HEALTH 200

#define KOOPAS_ANI_WALKING_LEFT 0
#define KOOPAS_ANI_WALKING_RIGHT 1
#define KOOPAS_ANI_HEALTH 2
#define KOOPAS_ANI_DIE 3

class CKoopas : public CGameObject
{
	int dame;
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

public:
	CKoopas();
	virtual void SetState(int state);
	virtual int GetDame() { return dame; };
};