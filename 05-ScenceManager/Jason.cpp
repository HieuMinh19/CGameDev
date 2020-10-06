#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Jason.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"
#include "Bullet.h"

CJason::CJason(float x, float y) : CGameObject()
{
	level = JASON_LEVEL_BIG;
	untouchable = 0;
	SetState(JASON_STATE_IDLE);

	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 
}

void CJason::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	if (!isJumping)
		vy += JASON_GRAVITY*dt;
	else
		vy += JUMP_GRAVITY * dt;
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=JASON_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > JASON_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		x += dx; 
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0; 
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 
		
		// block every object first!
		x += min_tx*dx + nx*0.4f;
		y += min_ty*dy + ny*0.4f;

		if (nx!=0) vx = 0;
		if (ny!=0) vy = 0;


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState()!= GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -JASON_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable==0)
					{
						if (goomba->GetState()!= GOOMBA_STATE_DIE)
						{
							if (level > JASON_LEVEL_SMALL)
							{
								level = JASON_LEVEL_SMALL;
								StartUntouchable();
							}
							else 
								SetState(JASON_STATE_DIE);
						}
					}
				}
			} // if Goomba
			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}
	//Jump checking
	if (isJumping) {
		if (GetTickCount() - jump_start > JASON_JUMP_TIME)
		{
			isJumping = FALSE;
			ResetJump();
		}
	}
	//Stand attack checking
	if (isAttackUp && !isStandAttack) {
		state = JASON_STATE_ATTACK_UP_RIGHT;
		if (GetTickCount() - attack_start > 450)
		{
			isStandAttack = TRUE;
			
		}
	}
	if (isStandAttack) {
		if (vx == 0) {
			if (!isJumping) {
				state = JASON_STATE_STAND_ATTACK_RIGHT;
			}
			else {
				state = JASON_STATE_JUMP_UP;
			}
		}
		else {
			if (nx > 0) {
				state = JASON_STATE_WALK_UP_RIGHT;
			}
		}
	}
	if (!isAttackUp) {
		state = JASON_STATE_IDLE;
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CJason::Render()
{
	int ani = -1;
	if (state == JASON_STATE_DIE)
		ani = JASON_ANI_DIE;
	else if (level == JASON_LEVEL_BIG) {
		if (vx == 0){
			if (!isJumping) {
				if (nx > 0) {
					ani = JASON_ANI_IDLE_RIGHT;
				}
				else ani = JASON_ANI_IDLE_LEFT;
			}
			if (isJumping) {
				if (nx > 0) {
					ani = JASON_ANI_JUMP_RIGHT;
				}
				else ani = JASON_ANI_JUMP_LEFT;
			}
		} else if (vx > 0) {
			if (isJumping) {
				if (nx > 0) {
					ani = JASON_ANI_JUMP_RIGHT;
				}
				else ani = JASON_ANI_JUMP_LEFT;
			}
			else {
				if (nx > 0) {
					ani = JASON_ANI_WALKING_RIGHT;
				}
				else {
					ani = JASON_ANI_WALKING_LEFT;
				}
			}
		}
		else {
			if (isJumping) {
				ani = JASON_ANI_JUMP_LEFT;
			}
			else {
				ani = JASON_ANI_WALKING_LEFT;
			}
		}
	}
	/*else if (level == MARIO_LEVEL_SMALL)
	{
		if (vx == 0)
		{
			if (nx>0) ani = MARIO_ANI_SMALL_IDLE_RIGHT;
			else ani = MARIO_ANI_SMALL_IDLE_LEFT;
		}
		else if (vx > 0)
			ani = MARIO_ANI_SMALL_WALKING_RIGHT;
		else ani = MARIO_ANI_SMALL_WALKING_LEFT;
	}*/

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox();
}

void CJason::SetState(int state){
	CGameObject::SetState(state);

	switch (state)
	{
	case JASON_STATE_WALKING_RIGHT:
		vx = JASON_WALKING_SPEED;
		nx = 1;
		break;
	case JASON_STATE_WALKING_LEFT: 
		vx = -JASON_WALKING_SPEED;
		nx = -1;
		break;
	case JASON_STATE_JUMP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -JASON_JUMP_SPEED_Y;
		break; 
	case JASON_STATE_JUMP_UP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -JASON_JUMP_SPEED_Y;
		break;
	case JASON_STATE_IDLE: 
		vx = 0;
		break;
	case JASON_STATE_DIE:
		vy = -JASON_DIE_DEFLECT_SPEED;
		break;
	case JASON_STATE_ATTACK_UP_RIGHT:
		level = JASON_LEVEL_ATTACK_UP;
		break;
	case JASON_STATE_WALK_UP_RIGHT:
		vx = JASON_WALKING_SPEED;
		nx = 1;
		break;
	}
}

void CJason::GetBoundingBox(float &left, float &top, float &right, float &bottom){
	float new_y;
	new_y = y + 15;
	left = x;
	top = new_y;

	if (level==JASON_LEVEL_BIG){
		right = x + JASON_BIG_BBOX_WIDTH;
		bottom = new_y + JASON_BIG_BBOX_HEIGHT;
	}
	else if (level == JASON_LEVEL_ATTACK_UP) {
		right = x + JASON_ATTACK_UP_BBOX_WIDTH;
		bottom = y + JASON_ATTACK_UP_BBOX_HEIGHT;
	}
	else {
		right = x + JASON_SMALL_BBOX_WIDTH;
		bottom = y + JASON_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CJason::Reset()
{
	SetState(JASON_STATE_IDLE);
	SetLevel(JASON_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(JASON_ANI_JUMP_RIGHT)->Reset();
}
void CJason::ResetJump() {
	isJumping = FALSE;
	SetState(JASON_STATE_IDLE);
	SetLevel(JASON_LEVEL_BIG);
	//SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(JASON_ANI_JUMP_RIGHT)->Reset();
	animation_set->at(JASON_ANI_JUMP_LEFT)->Reset();
	animation_set->at(JASON_ANI_JUMP_UP_RIGHT)->Reset();
	
}

void CJason::ResetAttackUp()
{
	SetState(JASON_STATE_IDLE);
	SetLevel(JASON_LEVEL_BIG);
	//SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(JASON_ANI_ATTACK_UP_RIGHT)->Reset();
}

void CJason::fire(vector<LPGAMEOBJECT> &objects)
{
	if (isAttackUp == true && isStandAttack != true) {
		return;
	}
    int ani_set_id = 6;

    CAnimationSets * animation_sets = CAnimationSets::GetInstance();
	
    CGameObject *obj = NULL;
	obj = new CBullet(nx);

    // General object setup
	if (isStandAttack == true) {
		obj->vx = 0;
		obj->vy = -BULLET_WALKING_SPEED;
		obj->SetPosition(x + JASON_BIG_BBOX_WIDTH /2, y);//code xấu
	}
	else if (nx > 0) {
		obj->SetPosition(x + JASON_BIG_BBOX_WIDTH, y + 16);//code xấu
	}
	else if (nx < 0) {
		obj->SetPosition(x , y + 16);
	}
    
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

    obj->SetAnimationSet(ani_set);
    objects.push_back(obj);
    
}