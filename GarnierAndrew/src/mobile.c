/*
 * mobile.c
 *
 *  Created on: 20 d�c. 2021
 *      Author: Garnier
 */
#include<stdio.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<assert.h>
#include"mobile.h"

struct s_mobile{

	SDL_Rect	frame;
	SDL_Point	speed;
	SDL_Color	color;
	int			live;
	struct s_mobile * pNext;
};

t_mobile*MobileCreate(int iPosX, int iPosY, int iSize,int iSpeedX, int iSpeedY, int iLive,SDL_Color color){

	t_mobile*pMobile = (t_mobile*)malloc(sizeof(t_mobile));
	if(pMobile==NULL){
		fprintf(stderr, "Memory Allocation Failed MobileCreate(): pMobile %s\n",SDL_GetError());
		return pMobile;
	}

	pMobile->frame=(SDL_Rect){
		.x=iPosX,
		.y=iPosY,
		.h=iSize,
		.w=iSize,
	};

	pMobile->speed=(SDL_Point){
		.x=iSpeedX,
		.y=iSpeedY,
	};

	pMobile->color=(SDL_Color){
		.r=color.r,
		.g=color.g,
		.b=color.b,
		.a=color.a,
	};

	pMobile->live = iLive;
	pMobile->pNext = NULL;

	return pMobile;
}

t_mobile*MobileDestroy(t_mobile*pMobile){
	free(pMobile);
	return NULL;
}

t_mobile*MobileDraw(const t_mobile*pMobile, SDL_Renderer*pRenderer){

		assert(pMobile);

		SDL_SetRenderDrawColor(pRenderer,pMobile->color.r,pMobile->color.g,pMobile->color.b,pMobile->color.a);

		if(pMobile->live <100){
			SDL_RenderDrawRect(pRenderer,&pMobile->frame);
		}
		else{
			SDL_RenderFillRect(pRenderer,&pMobile->frame);
		}

		return NULL;
}

t_mobile*MobileMove(t_mobile*pMobile, const SDL_Rect*pArea){

	assert(pMobile);

	pMobile->frame.x += pMobile->speed.x;
	pMobile->frame.y += pMobile->speed.y;

	if(pMobile->frame.x < pArea->x){
		pMobile->frame.x = pArea->x;
		pMobile->speed.x *=-1;
	}
	if(pMobile->frame.y < pArea->y){
		pMobile->frame.y = pArea->y;
		pMobile->speed.y *=-1;
	}
	if(pMobile->frame.x+ pMobile->frame.w >=pArea->w + pArea->x){
		pMobile->frame.x = pArea->w + pArea->x- pMobile->frame.w ;
		pMobile->speed.x *=-1;
	}
	if(pMobile->frame.y+ pMobile->frame.h >=pArea->h + pArea->y){
		pMobile->frame.y = pArea->h + pArea->y- pMobile->frame.w;
		pMobile->speed.y *=-1;
	}


	return NULL;
}

int MobileGrowOlder(t_mobile*pMobile){

	assert(pMobile);

	if(pMobile->live){

		pMobile->live-=1;
		return 0;
	}
	return 1;
}



int MobileIsThis(const t_mobile*pMobile, SDL_MouseMotionEvent *pMouseEvent){

	assert(pMobile);

	return SDL_PointInRect(&(SDL_Point){pMouseEvent->x,pMouseEvent->y},&pMobile->frame);
}





//----------------//


























