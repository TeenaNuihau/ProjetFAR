#include<stdlib.h>
#include<stdio.h>

#include "SortList.h"

/******************************************************************************/

void Insert(slist **sl, int Val) {
        slist *tmp = NULL;
        slist *csl = *sl;
        slist *elem = malloc(sizeof(slist));
        if(!elem) exit(EXIT_FAILURE);
        elem->valeur = Val;
        while(csl && csl->valeur < Val)
        {
                tmp = csl;
                csl = csl->suiv;
        }
        elem->suiv = csl;
        if(tmp) tmp->suiv = elem;
        else *sl = elem;
}
/******************************************************************************/

int Pop(slist **sl) {
        int Val;
        slist *tmp;
        if(!*sl) return -1;     
        tmp = (*sl)->suiv;
        Val = (*sl)->valeur;
        free(*sl);
        *sl = tmp;             
        return Val;            
}

/******************************************************************************/

void Clear(slist **sl){
        slist *tmp;
        while(*sl)
        {
                tmp = (*sl)->suiv;
                free(*sl);
                *sl = tmp;
        }
}
/******************************************************************************/

int Length(slist *sl)
{
        int n=0;
        while(sl)
        {
                n++;
                sl = sl->suiv;
        }
        return n;
}

/******************************************************************************/

void View(slist *sl)
{
       while(sl)
        {
                printf("%d\n",sl->valeur);
                sl = sl->suiv;
        }
}