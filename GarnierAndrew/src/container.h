/*
 * container.h
 *
 *  Created on: 6 janv. 2022
 *      Author: Garnier
 */


typedef	void*(*t_ptfV)(void*);
typedef	void*(*t_ptfVV)(void*,void*);

typedef struct s_container t_container;


t_container*ContainerNew(t_ptfV cpDeleteItemFunc);
t_container*ContainerDel(t_container *pContainer);


t_container*ContainerFlush(t_container*pContainer);

void*ContainerPushback(t_container*pContainer, void*pItem);
void*ContainerPushFront(t_container*pContainer, void*pItem);
void*ContainerPushhat(t_container*pContainer, void*pItem, size_t szAt);



void*ContainerParse(t_container*pContainer, t_ptfVV pParseFunc, void*pParam);
void*ContainerParseDelIf(t_container* pContainer,t_ptfVV pParseFunc, void* pParam);



