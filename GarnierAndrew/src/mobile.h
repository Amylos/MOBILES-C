/*
 * mobile.h
 *
 *  Created on: 20 déc. 2021
 *      Author: Garnier
 */

typedef struct s_mobile t_mobile;


t_mobile*MobileCreate(int iPosX, int iPosY, int iSize,int iSpeedX, int iSpeedY, int iLive,SDL_Color color);
t_mobile*MobileDestroy(t_mobile*pMobile);

t_mobile*MobileDraw(const t_mobile*pMobile, SDL_Renderer*prenderer);
t_mobile*MobileMove(t_mobile*pMobile, const SDL_Rect*pArea);

int MobileGrowOlder(t_mobile*pMobile);
int MobileIsThis(const t_mobile*pMobile, SDL_MouseMotionEvent *pMouseEvent);


int mobileFind(t_mobile*pMobile,int pInt);




