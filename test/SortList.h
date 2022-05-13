#ifndef CGI_SORTLIST_H
#define CGI_SORTLIST_H

 /*  Structure représentant un élément de la liste. */

       typedef struct slist {
              int valeur;
              struct slist *suiv;
       } slist ;

#ifdef __cplusplus
extern "C" {
#endif

 /*  Insert insert une valeur dans la liste. */

        void Insert(slist **sl, int Val);


 /*  Pop retire la dernière valeur de la liste. */

        int Pop(slist **sl);


 /*  Clear vide la liste. */

        void Clear(slist **sl);


 /*  Lenght retourne le nombre d'éléments de la liste. */

        int Length(slist *sl);


 /*  Affiche la totalité de la liste en commençant par le sommet. */

        void View(slist *sl);

#ifdef __cplusplus
}
#endif

#endif