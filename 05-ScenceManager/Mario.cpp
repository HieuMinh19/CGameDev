#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"
#include "Bullet.h"
#include "Koopas.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_BIG;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	health = MARIO_HEALTH_MAX;
	maxHealth = MARIO_HEALTH_MAX;

	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);
	// Simple fall down
	if (!isJumping)
		vy += MARIO_GRAVITY*dt;
	else
		vy += JUMP_GRAVITY * dt;
//=======
//	vy += MARIO_GRAVITY*dt;
//>>>>>>> week1/items
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
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
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable==0)
					{
						if (goomba->GetState()!= GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else 
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			} // if Goomba
			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
			else if (dynamic_cast<CKoopas *>(e->obj))
			{
				CKoopas *koopas = dynamic_cast<CKoopas *>(e->obj);
				if (untouchable == 0)
				{	
					if (koopas->GetState() == KOOPAS_STATE_HEALTH)
					{
						health += 100;
						if (health > maxHealth)
							health = maxHealth;
						koopas->SetState(KOOPAS_STATE_DIE);
						DebugOut(L"[ERROR] M�u %i \n", health);
					}
					else {
						health -= koopas->GetDame();
						if (health < 0) {
							SetState(MARIO_STATE_DIE);
						}
						StartUntouchable();
						DebugOut(L"[ERROR] M�u %i \n", health);
					}
				}
			}
		}
	}
	//Jump checking
	if (isJumping) {
		if (GetTickCount() - jump_start > MARIO_JUMP_TIME)
		{
			isJumping = FALSE;
			ResetJump();
		}
	}
	//Stand attack checking
	if (isAttackUp && !isStandAttack) {
		state = MARIO_STATE_ATTACK_UP_RIGHT;
		if (GetTickCount() - attack_start > 450)
		{
			isStandAttack = TRUE;
			
		}
	}
	if (isStandAttack) {
		if (vx == 0) {
			if (!isJumping) {
				state = MARIO_STATE_STAND_ATTACK_RIGHT;
			}
			else {
				state = MARIO_STATE_JUMP_UP;
			}
		}
		else {
			if (nx > 0) {
				state = MARIO_STATE_WALK_UP_RIGHT;
			}
		}
	}
	if (!isAttackUp) {
		state = MARIO_STATE_IDLE;
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else if (level == MARIO_LEVEL_BIG)
	{
		if (vx == 0)
		{
			if (!isJumping) {
				if (nx > 0) {
					if (state == MARIO_STATE_IDLE)
						ani = MARIO_ANI_BIG_IDLE_RIGHT;
					else if(state == MARIO_STATE_ATTACK_UP_RIGHT) 
						ani = MARIO_ANI_ATTACK_UP_RIGHT;
					else if (state == MARIO_STATE_STAND_ATTACK_RIGHT) {
						ani = MARIO_ANI_STAND_ATTACK_RIGHT;
					}
					else ani = MARIO_ANI_BIG_IDLE_RIGHT;
				}
				else ani = MARIO_ANI_BIG_IDLE_LEFT;
			}
			if (isJumping) {
				if (nx > 0) {
					if(state == MARIO_STATE_JUMP_UP){
						ani = MARIO_ANI_JUMP_UP_RIGHT;
					}
					else {
						ani = MARIO_ANI_JUMP_RIGHT;
					}
				}
				else ani = MARIO_ANI_JUMP_LEFT;
			}
		}
		else if (vx > 0) {
			if(state == MARIO_STATE_WALK_UP_RIGHT){ 
				ani = MARIO_ANI_WALK_UP_RIGHT;
			}
			else {
				ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}
		}
		else {
			ani = MARIO_ANI_BIG_WALKING_LEFT;
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

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		vx = MARIO_WALKING_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT: 
		vx = -MARIO_WALKING_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -MARIO_JUMP_SPEED_Y;
		break; 
	case MARIO_STATE_JUMP_UP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_IDLE: 
		vx = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_ATTACK_UP_RIGHT:
		level = MARIO_LEVEL_ATTACK_UP;
		break;
	case MARIO_STATE_WALK_UP_RIGHT:
		vx = MARIO_WALKING_SPEED;
		nx = 1;
		break;
	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	float new_y;
	new_y = y + 15;
	left = x;
	top = new_y;

	if (level==MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = new_y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_ATTACK_UP) {
		right = x + MARIO_ATTACK_UP_BBOX_WIDTH;
		bottom = y + MARIO_ATTACK_UP_BBOX_HEIGHT;
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(MARIO_ANI_JUMP_RIGHT)->Reset();
}
void CMario::ResetJump()
{
	isJumping = FALSE;
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	//SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(MARIO_ANI_JUMP_RIGHT)->Reset();
	animation_set->at(MARIO_ANI_JUMP_LEFT)->Reset();
	animation_set->at(MARIO_ANI_JUMP_UP_RIGHT)->Reset();
	
}

void CMario::ResetAttackUp()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	//SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	//reset sate jump
	animation_set->at(MARIO_ANI_ATTACK_UP_RIGHT)->Reset();
}

void CMario::fire(vector<LPGAMEOBJECT> &objects)
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
		obj->SetPosition(x + MARIO_BIG_BBOX_WIDTH /2, y);//code xấu
	}
	else if (nx > 0) {
		obj->SetPosition(x + MARIO_BIG_BBOX_WIDTH, y + 16);//code xấu
	}
	else if (nx < 0) {
		obj->SetPosition(x , y + 16);
	}
    
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

    obj->SetAnimationSet(ani_set);
    objects.push_back(obj);
    
}
void CMario::fire(vector<LPGAMEOBJECT> &objects)
{
	
	int ani_set_id = 6;

	CAnimationSets * animation_sets = CAnimationSets::GetInstance();

	CGameObject *obj = NULL;
	obj = new CBullet(nx);

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
	
}
