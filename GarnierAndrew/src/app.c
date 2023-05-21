/*
 * app.c
 *
 *  Created on: 20 déc. 2021
 *      Author: Garnier
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<SDL2/SDL.h>
#include<assert.h>


#include"mobile.h"
#include "container.h"
#include"app.h"


/*********************  Bits Mask Manipulation Macros  *******************/
#define mBitsSet(f,m)	((f) |= (m))
#define	mBitsClr(f,m)	((f)&=(~(m)))
#define	mBitsTgl(f,m)	((f)^=(m))
#define	mBitsMsk(f,m)	((f)&(m))
#define	mIsBitsSet(f,m)	(((f)&(m))==(m))
#define	mIsBitClr(f,m)	(((~(f))&(m))==(m))

/**********  Enumerate Application Status Masks Type Definition ***********/
typedef enum eppFLagsMasks{
	ST_APP_ALL_CLEARED		=	0x00000000,
	ST_APP_INIT_FAILED		=	0x80000000,
	ST_APP_INIT_INITIATED	=	0x00000001,
	ST_APP_PAUSED			=	0x00000002,
	ST_APP_MULTI_CREATION	=	0x00000004,
}t_appFLagsMasks;

/********************  Application Structure Definition ******************/
struct s_app{
	t_appFLagsMasks	uStatus;
	SDL_Window		*pWindow;
	SDL_Renderer	*pRenderer;
	SDL_TimerID		timerID;
	SDL_Event		sEvent;
	/*-----------------------------*/
	SDL_Rect		sceneArea;
	int				interval;
	int				iRun;
	int 			iNbMobile;
	t_mobile 		*pMobiles;
	t_container		*pContainer;

	SDL_Point 		*point;
};

/*****************  Animation Callback Function Prototype ****************/

Uint32 _AppTimerCallBack(Uint32 interval, t_app*pApp);

/****************   Display Scene on Click Function Prototype ************/

void _AppDrawScene(SDL_Renderer*pRenderer,SDL_Color colorWindow,SDL_Color colorArea,SDL_Rect*sceneArea, t_app*pApp);

/*************** Public Application Function Implementation **************/

t_app*AppCreate(void){
	srand((unsigned int) time(NULL));
/*****************  Memory Allocation to pApp ***********************/
	t_app*pApp = (t_app*)malloc(sizeof(t_app));

	assert(pApp);

//	if(pApp == NULL){
//		printf("MEMORY ALLOCATION FAILED : AppCreate() : pApp");
//		return pApp;
//	}



//
//	t_container*pContainer = ContainerNew((t_ptfV)MobileDestroy);
//
//	assert(pContainer);

	*pApp =(t_app){

		.uStatus 	= ST_APP_ALL_CLEARED,
		.pWindow 	= NULL,
		.pRenderer 	= NULL,
		.timerID	= (int)NULL,
		.iRun		= 1,
		.iNbMobile	=APP_MOBILE_NB_MAX,
		.pMobiles	= NULL,
		.pContainer	=  ContainerNew((t_ptfV)MobileDestroy),
		.sceneArea=(SDL_Rect){
				.x=APP_WINDOW_PADDING_HRZ,
				.y=APP_WINDOW_PADDING_TOP,
				.w=APP_WINDOW_WIDTH-APP_WINDOW_PADDING_HRZ*2,
				.h=APP_WINDOW_HEIGHT-APP_WINDOW_PADDING_TOP-APP_WINDOW_PADDING_BOT,

			},
	};




	for(int k=0;k<APP_MOBILE_NB_MAX;k++){
//		pApp->mobiles[k]=NULL;
//		ContainerPushback(pApp->pContainer, NULL);
		// NO NEED TO DO THAT BECAUSE NO FIELD ARE EXISTING YET
	}


/******************************************************************/
	if(SDL_Init(SDL_INIT_VIDEO) !=0){

			fprintf(stderr, "SDL video initialization failed %s \n",SDL_GetError());
			mBitsSet(pApp->uStatus,ST_APP_INIT_FAILED);
			return pApp;
		}
	else{
		mBitsSet(pApp->uStatus,ST_APP_INIT_INITIATED);
	}

/*************************  Window Creation ************************/
	pApp->pWindow = SDL_CreateWindow(
			APP_WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			APP_WINDOW_WIDTH,
			APP_WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
	);

	if(pApp->pWindow==NULL){
		fprintf(stderr, "Failed to create window %s\n",SDL_GetError());
		mBitsSet(pApp->uStatus,ST_APP_INIT_FAILED);
		return pApp;
	}

/**********************  Renderer Initialization  *********************/

	pApp->pRenderer = SDL_CreateRenderer(pApp->pWindow, -1, SDL_RENDERER_ACCELERATED);
	if(pApp->pRenderer == NULL){
		fprintf(stderr,"Failed to create accelerated renderer. \n");
		pApp->pRenderer = SDL_CreateRenderer(pApp->pWindow, -1, SDL_RENDERER_SOFTWARE);
		if(pApp->pRenderer == NULL){
			fprintf(stderr,"Failed to create software renderer. \n");
			mBitsSet(pApp->uStatus,ST_APP_INIT_FAILED);
			return pApp;
		}
		fprintf(stderr,"Software renderer created instead!\n");
	}

	return (pApp);
}


t_app*AppDestroy(t_app*pApp){


	if(pApp == NULL) return NULL;
	if((pApp->timerID))	SDL_RemoveTimer(pApp->timerID);
	ContainerParse(pApp->pContainer, (t_ptfVV)MobileDestroy,NULL);
	if(pApp->pContainer) free(pApp->pContainer);
	if(pApp->pRenderer)	SDL_DestroyRenderer(pApp->pRenderer);
	if(pApp->pWindow)	SDL_DestroyWindow(pApp->pWindow);
	if(mIsBitsSet(pApp->uStatus,ST_APP_INIT_INITIATED))	SDL_Quit();
	free(pApp);

	return NULL;
}


int AppRun(t_app*pApp){

	if(pApp==NULL || mIsBitsSet(pApp->uStatus,ST_APP_INIT_FAILED)){
		return EXIT_FAILURE;
	}

	for(int k=0;k<APP_MOBILE_NB_MAX;k++){	// First creation of mobiles

		ContainerPushFront(pApp->pContainer,MobileCreate(rand()%(APP_WINDOW_WIDTH-(APP_WINDOW_PADDING_HRZ*2)-APP_MOBILE_SIZE)+APP_WINDOW_PADDING_HRZ,
														  rand()%(APP_WINDOW_HEIGHT-APP_WINDOW_PADDING_BOT-APP_WINDOW_PADDING_TOP-APP_MOBILE_SIZE)+APP_WINDOW_PADDING_TOP,
														  APP_MOBILE_SIZE,
														  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
														  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
														  rand()%(APP_MOBILE_LIVE_MAX - APP_MOBILE_LIVE_MIN +1)+APP_MOBILE_LIVE_MIN,
														  (SDL_Color){rand()%(255-100+1)+100,rand()%(255-100+1)+100,rand()%(255-100+1)+100,255}
		));
	}

	pApp->timerID= SDL_AddTimer(APP_MOBILE_ANIMATE_RATE, (SDL_TimerCallback) _AppTimerCallBack,pApp);

	while(pApp->iRun){
			while(SDL_PollEvent(&pApp->sEvent)){
				switch(pApp->sEvent.type){
				case SDL_MOUSEBUTTONDOWN:
					switch(pApp->sEvent.button.button){
					case SDL_BUTTON_LEFT:	// Mobile Creation in case of click left on it and if the array is not full

							ContainerPushFront(pApp->pContainer,MobileCreate(pApp->sEvent.motion.x,
																		  pApp->sEvent.motion.y,
																		  APP_MOBILE_SIZE*1.2,
																		  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
																		  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
																		  rand()%(APP_MOBILE_LIVE_MAX - APP_MOBILE_LIVE_MIN +1)+APP_MOBILE_LIVE_MIN,
																		  (SDL_Color){rand()%(255-100+1)+100,rand()%(255-100+1)+100,rand()%(255-100+1)+100,255}
																		));


							if(mIsBitsSet(pApp->uStatus,ST_APP_PAUSED)){
								_AppDrawScene(pApp->pRenderer,APP_WINDOW_COLOR_BKGND,APP_WINDOW_COLOR_DELIMITING,&pApp->sceneArea,pApp);
							}

						break;
					case SDL_BUTTON_RIGHT:
						ContainerParseDelIf(pApp->pContainer, (t_ptfVV)MobileIsThis,&pApp->sEvent);

						if(mIsBitsSet(pApp->uStatus,ST_APP_PAUSED)){
							_AppDrawScene(pApp->pRenderer,APP_WINDOW_COLOR_BKGND,APP_WINDOW_COLOR_DELIMITING,&pApp->sceneArea,pApp);
						}
						break;
					default:
						break;
					break;
					}
					break;
				case SDL_QUIT:
					pApp->iRun = 0;
					break;
				case SDL_KEYUP:
					switch(pApp->sEvent.key.keysym.sym){
					case SDLK_s:
						mBitsTgl(pApp->uStatus,ST_APP_MULTI_CREATION);
						break;
					case SDLK_ESCAPE:
						pApp->sEvent.type = SDL_QUIT;
						SDL_PushEvent(&pApp->sEvent);
						break;
					case SDLK_SPACE:
						mBitsTgl(pApp->uStatus,ST_APP_PAUSED);
						break;

					default:
						break;
					}
					break;
				default:
					break;
				}
			}
		}
	return 0;
}



/****************** Animating Callback Function Implementation  ***************/

Uint32 _AppTimerCallBack(Uint32 interval, t_app*pApp){

	if(mIsBitsSet(pApp->uStatus,ST_APP_PAUSED)){
		SDL_SetWindowTitle(pApp->pWindow, APP_WINDOW_TITLE_PAUSED);
		return interval;
	}
	else{

		SDL_SetWindowTitle(pApp->pWindow, APP_WINDOW_TITLE);
	}

	SDL_SetRenderDrawColor(
			pApp->pRenderer,
			APP_WINDOW_COLOR_BKGND.r,
			APP_WINDOW_COLOR_BKGND.g,
			APP_WINDOW_COLOR_BKGND.b,
			APP_WINDOW_COLOR_BKGND.a);

	SDL_RenderClear(pApp->pRenderer);
	SDL_SetRenderDrawColor(pApp->pRenderer,APP_WINDOW_COLOR_DELIMITING.r,APP_WINDOW_COLOR_DELIMITING.g,APP_WINDOW_COLOR_DELIMITING.b,APP_WINDOW_COLOR_DELIMITING.a);
	SDL_RenderDrawRect(pApp->pRenderer,&pApp->sceneArea);


	ContainerParse(pApp->pContainer,(t_ptfVV)MobileDraw,pApp->pRenderer);
	ContainerParse(pApp->pContainer,(t_ptfVV)MobileMove, &pApp->sceneArea);
	ContainerParseDelIf(pApp->pContainer,(t_ptfVV)MobileGrowOlder, NULL);





//		if(pApp->mobiles[k]){
//			MobileMove(pApp->mobiles[k],&pApp->sceneArea);
//			MobileDraw(pApp->mobiles[k],pApp->pRenderer);
//
//			if(MobileGrowOlder(pApp->mobiles[k])==0){
//				pApp->mobiles[k]=MobileDestroy(pApp->mobiles[k]);
//				pApp->iNbMobile -=1;
//			}
//		}

//		if(pApp->iNbMobile==0){
//
//			for(int k =0;k<rand()%(APP_MOBILE_NB_MAX+(APP_MOBILE_NB_MAX/2))+(APP_MOBILE_NB_MAX/2);k++){
//				pApp->mobiles[k]=MobileCreate(rand()%(APP_WINDOW_WIDTH-(APP_WINDOW_PADDING_HRZ*2)-APP_MOBILE_SIZE)+APP_WINDOW_PADDING_HRZ,
//													  rand()%(APP_WINDOW_HEIGHT-APP_WINDOW_PADDING_BOT-APP_WINDOW_PADDING_TOP-APP_MOBILE_SIZE)+APP_WINDOW_PADDING_TOP,
//													  APP_MOBILE_SIZE,
//													  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
//													  rand()%(APP_MOBILE_SPEED_MAX*2+1)-APP_MOBILE_SPEED_MAX,
//													  rand()%(APP_MOBILE_LIVE_MAX - APP_MOBILE_LIVE_MIN +1)+APP_MOBILE_LIVE_MIN,
//													  (SDL_Color){rand()%(255-100+1)+100,rand()%(255-100+1)+100,rand()%(255-100+1)+100,255}
//														);
//				pApp->iNbMobile+=1;
//			}
//		}


	SDL_RenderPresent(pApp->pRenderer);
	return interval;
}

/************************  Private Function IMPLEMENTATION ***************************/

void _AppDrawScene(SDL_Renderer*pRenderer,SDL_Color colorWindow,SDL_Color colorArea,SDL_Rect*sceneArea, t_app*pApp){
				SDL_SetRenderDrawColor(pRenderer, colorWindow.r, colorWindow.g, colorWindow.b, colorWindow.a);
				SDL_RenderClear(pRenderer);

				SDL_SetRenderDrawColor(pRenderer,colorArea.r,colorArea.g,colorArea.b,colorArea.a);
				SDL_RenderDrawRect(pRenderer,sceneArea);



					ContainerParse(pApp->pContainer,(t_ptfVV)MobileDraw,pApp->pRenderer);

				SDL_RenderPresent(pApp->pRenderer);
}

