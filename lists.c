/*  Filenames   : lists.c, lists.h                                       *
 *  Author(s)   : Michael Becker,                                        *
 *  Date        : 12.04.2000                                             *
 *  Description : Listen-Operationen                                     *
 *                                                                       *
 *  History     :                                                        *
 *  Date        | Action                              | Autor            *
 * -------------+-------------------------------------+------------------*
 *  12.04.2000  | Erstellung der Datei                | MBecker          *
 *  18.07.2000  | list_t_cmp hinzugefuegt             | "                *
 *  13.08.2001  | find_value-Funktion erweitert       | "                *
 *  27.10.2001  | Bug in concat_lists gefixt          | "                */

/* Diese Library steht unter der BSD-Lizenz:

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.</li>

   - My name may not be used to endorse or promote products derived from this
     software without specific prior written permission.</li>

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS 
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
   THE POSSIBILITY OF SUCH DAMAGE.


   Zu Deutsch: Diese Software darf frei kopiert werden. Sie darf fuer
   kommerzielle sowie fuer freie Programme verwendet werden, sofern erwaehnt 
   wird, dass diese Library unter dieser Lizenz steht.

   Weiterhin bestaetigt der Autor hiermit, dass er die library nach bestem
   Wissen und Gewissen erstellt hat. Er uebernimmt jedoch keinerlei 
   Verantwortung oder Garantien fuer was auch immer.

   Michael Becker <michael.www@ijon.de"> 13.8.2001 */

/*****************************************************************************/


#include <stdio.h>
#include <malloc.h>


/* Listen sind hier doppelt verkettete Listen mit Kopfelement. Der Inhalt 
   eines Listenelements ist ein void*-Pointer. Der Inhalt des Kopf-
   elementes ist NULL. 
   Der next-Zeiger des letzten Elements zeigt auf NULL.
   der prev-Zeiger des ersten Elements zeigt auf den letzten Listeneintrag.
     (bzw. auf das Kopfelement, wenn die Liste leer ist).
*/

#include "lists.h"



/* Funktion:      list_t *new_list();
   Beschreibung : legt eine neue Liste an. Ein Zeiger auf den Listenkopf 
                  wird zurueckgegeben. */
list_t *new_list ()
{
  list_t *nl=(list_t*)malloc(sizeof(list_t));
  /* hier fehlt noch eine Fehlerabfrage */
  nl->val=NULL;
  nl->next=NULL;
  nl->prev=nl;
  return nl;
}



/* Funktion:      list_t* insert_list_element (list_t* head, list_t* pos, 
                                               void *v);
   Beschreibung : Fuegt v hinter dem Listenelement pos der liste head ein.
                  Insbesondere fuegt 
                       insert_list_element (l,l->prev, v);
                  v am Ende der Liste ein. Und
                       insert_list_element (l,l, v);
                  am Anfang der Liste.
                  Gibt pos zurueck. */
list_t* insert_list_element (list_t* head, list_t* pos, void *v)
{
  list_t *tmp=(list_t*)malloc(sizeof(list_t));
  /* Fehlerabfrage fehlt */
  tmp->val=v;
  tmp->next=pos->next;
  tmp->prev=pos;
  if (pos->next) pos->next->prev=tmp;
  else head->prev=tmp;
  pos->next=tmp;
  return pos;
}


/* Funktion:      list_t* find_lastlesser_element (list_t *head, void *v, 
                                    int (*cmp_func)(void*, void*));
   Beschreibung : Gibt in einer sortierten Liste einen Zeiger auf das 
                  Element zurueck, das noch kleiner als v ist (cmp_func 
                  ist <0), aber so, dass das naechste Element nicht mehr 
                  kleiner als v ist (cmp_func>=0) (oder das Listenende 
                  erreicht ist).
                  Allgemein: Der zurueckgegebene Zeiger zeigt auf das letzte
                  Listenelement, fuer das cmp_func(*,v) noch <0 ist. */
list_t* find_lastlesser_element (list_t *head, void *v, 
    int (*cmp_func)(void*, void*))
{
  list_t* pos=head;
  while (pos->next&&(((*cmp_func)(pos->next->val,v))<0)) pos=pos->next;
  return pos;
}


/* Funktion:      list_t* insert_sorted_element (list_t* head, void *v, 
                                        int (*cmp_func)(void*, void*));
   Beschreibung : fuegt einen value v in eine geordnete Liste head ein. 
                  Als Vergleichsfunktion wird cmp_func benutzt.
                  Gibt einen Zeiger auf das Element der Liste zurueck, 
                  hinter dem das neue Element eingefuegt wurde. */
list_t* insert_sorted_element (list_t* head, void *v,
                                int (*cmp_func)(void*, void*))
{
  return insert_list_element (head, 
              find_lastlesser_element(head,v,cmp_func), v);
}




/* Funktion:      int erase_list_element (list_t* head, list_t* pos, 
                                          void (*free_func)(void *));
   Beschreibung : loescht das Listenelement an Position pos aus der Liste 
                  head. Ist free_func!=NULL, so wird zum Loeschen des 
                  values diese Funktion aufgerufen. Ist free_func==NULL, so
                  wird der val nicht geloescht. */
int erase_list_element (list_t* head, list_t* pos, 
    void (*free_func)(void *))
{
  if (!(pos&&head)) return -1;
  if (pos==head) return -1;
  if (free_func&&pos->val) (*free_func)(pos->val);
  pos->prev->next=pos->next;
  if (pos->next) pos->next->prev=pos->prev;
  else head->prev=pos->prev;
  free(pos);
  return 0;
}



/* Funktion:      list_t *find_value (list_t *head, void *v, 
                                int (*cmp_func)(void*, void*));
   Beschreibung : Sucht den Wert v in der (ungeordneten) Liste l und 
                  gibt einen Zeiger auf das Listenelement zurueck. Gibt 
                  es den Wert nicht, so wird NULL zurueckgegeben. Um 
                  festzustellen, ob zwei Werte gleich sind, wird die 
                  Funktion cmp_func verwendet, die bei Gleichheit 0 ist. 
                  Ist cmp_func==NULL, so wird auf Gleichheit der val-Zeiger
                  getestet. */
list_t *find_value (list_t *head, void *v, int (*cmp_func)(void*, void*))
{
  while ((head=head->next) &&
         (cmp_func ? (*cmp_func)(head->val,v) : (head->val!=v)));
  return head;
}



/* Funktion:      list_t *find_sorted_value (list_t *head, void *v, 
                                int (*cmp_func)(void*, void*));
   Beschreibung : Sucht den Wert v in der (geordneten) Liste l und 
                  gibt einen Zeiger auf das Listenelement zurueck. Gibt 
                  es den Wert nicht, so wird NULL zurueckgegeben. Fuer die
                  Vergleiche wird die Funktion cmp_func verwendet. */
list_t *find_sorted_value (list_t *head, void *v, 
                                   int (*cmp_func)(void*, void*))
{
  list_t* tmp = find_lastlesser_element (head,v,cmp_func);
  if (!tmp->next) return NULL;
  if (cmp_func(tmp->next->val,v)) return NULL;
  else return tmp->next;
}



/* Funktion:      void clear_list (list_t* head, void (*free_func)(void *));
   Beschreibung : Loescht die gesamte Liste head samt Kopfelement. Ist 
                  free_func!=NULL, so wird zum Loeschen der values diese 
                  Funktion aufgerufen. */
void clear_list (list_t* head, void (*free_func)(void *))
{
  list_t *tmp;
  while (head) {
    tmp=head->next;
    if (free_func&&head->val) (*free_func)(head->val);
    free(head);
    head=tmp;
  }
}


/* Funktion:      list_t *copy_list (list_t *head, void* (*copy_func)(void*));
   Beschreibung : kopiert die uebergebene Liste. Ist copy_func==NULL, 
                  so zeigen die val-pointer der neuen Liste auf dieselben 
                  Adressen wie die der alten Liste. Ansonsten wird 
                  copy_func zum kopieren der Werte verwendet. */
list_t *copy_list (list_t *head, void* (*copy_func)(void*))
{
  list_t *newlist=new_list();
  while((head=head->next)!=NULL)
    insert_list_element (newlist, newlist->prev,
           copy_func ? copy_func(head->val) : head->val );
  return newlist;
}


/* Funktion:      int list_size (list_t *head);
   Beschreibung : gibt die Laenge der Liste (ohne Kopfelement) zurueck.
                  Ist die Liste noch nicht angelegt (d.h. head==NULL, 
                  so wird -1 zurueckgegeben. */
int list_size (list_t *head)
{
  int n=0;
  if (!head) return -1;
  while ((head=head->next)) n++;
  return n;
}

/* Funktionen:    list_t* next_n (list_t* head, list_t* pos, int n);
                  list_t* prev_n (list_t* head, list_t* pos, int n);
   Beschreibung : zaehlt von pos an n Listenelemente weiter bzw. zurueck und
                  gibt einen Zeiger auf diese Position zurueck. Wird das 
                  Listenende (bzw. der Listenanfang) ueberschritten, so wird
                  NULL zurueckgegeben. (Man beachte, dass beim rueckwaerts-
                  laufen nie das Kopfelement zurueckgegeben wird. */
list_t* next_n (list_t* head, list_t* pos, int n)
{
  while ((pos)&&(n--)) pos=pos->next;
  return pos;
}

list_t* prev_n (list_t* head, list_t* pos, int n)
{
  while ((pos->prev->next)&&(n--)) pos=pos->prev;
  return (pos->prev->next);
}

/* Funktion:      int list_t_cmp (list_t* head, list_t* l1, list_t* l2)
   Beschreibung : schaut, ob l1 oder l2 zuerst in der Liste vorkommt.
                  Im Fall l1==l2 wird 0 zurueckgegeben. Ansonsten wird der
                  erste Eintrag, der l1 oder l2 ist, gesucht. Ist es l1, so 
                  wird -1 zurueckgegeben, ansonsten +1.
                  wird weder l1 noch l2 in der Liste gefunden, so wird 0
                  zurueckgegeben. */
int list_t_cmp (list_t* head, list_t* l1, list_t* l2)
{
  if (l1==l2) return 0;
  while ((head=head->next))
    if (head==l1) return -1;
    else if (head==l2) return +1;
  return 0;
}


/* Funktion:      void concat_lists (list_t *l1, list_t *l2);
   Beschreibung : haengt die Liste l2 an die Liste l1 an. Gibt den
                  Speicherplatz fuer das Kopfelement von l2 wieder frei. */
void concat_lists (list_t *l1, list_t* l2)
{
  if (l2->next) {
    l1->prev->next = l2->next;
    l2->next->prev = l1->prev;
    l1->prev       = l2->prev;
  }
  free(l2);
}




/* Funktion:      list_t*  splice_list (list_t* l, list_t* splitpos);
   Beschreibung : Teilt die Liste l in zwei Listen auf: Die Liste l enthaelt
                  danach alle Listenelement bis splitpos einschliesslich.
                  Die als  Rueckgabewert uebergebene Liste enthaelt alle
                  Element ab splitpos (ausschliesslich). */
list_t*  splice_list (list_t* l, list_t* splitpos)
{
  list_t* newlist=new_list();
  newlist->next=splitpos->next;

  if (splitpos->next) {
    splitpos->next->prev=newlist;
    newlist->prev=l->prev;
  }
  else newlist->prev=newlist;

  splitpos->next=NULL;
  l->prev=splitpos;
  return newlist;
}



/* Funktion:      list_t* merge_sorted_lists (list_t *l1, list_t *l2,
                               int (*cmp_func)(void*, void*));
   Beschreibung : fuegt die beiden mit cmp_func sortierten Listen zu 
                  einer einzigen sortierten Liste zusammen.
                  Hierbei werden nur die Zeiger neu gesetzt. Die Listen-
                  elemente der neuen Liste sind genau die der alten Listen.
                  Die Kopfelemente der alten Listen werden freigegeben. */
list_t* merge_sorted_lists (list_t *l1, list_t* l2, 
			    int (*cmp_func)(void*, void*))
{
  list_t *i1=l1->next, *i2=l2->next, *i;
  list_t *lhead=new_list(), *l;

  l=lhead;
  while (i1 || i2) {
    if ((!i1) || ((i1&&i2)&&(cmp_func(i2->val,i1->val))<0)) {
      i =i1;
      i1=i2;
      i2=i;
    }
    /* jetzt i1 hinten an die Liste l anfuegen */
    l->next=i1;
    i1->prev=l;
    l=l->next;
    i1=i1->next;
  }
  lhead->prev=l;
  free(l1);
  free(l2);
  return lhead;
}




/* Funktion:      list_t* merge_sorted_sublists (list_t* head, 
                          list_t* startpos, list_t* splitpos, list_t* endpos, 
                          int (*cmp_func)(void*, void*));
   Beschreibung : Enthaelt eine Liste zwei hintereinanderfolgende,
                  sortierte Teillisten, so kann die Liste mit Hilfe dieser
                  Funktion so umsortiert werden, dass aus den zwei
                  sortierten Teillisten eine einzige, groessere, sortierte
                  Teilliste wird.
                  Die erste Teilliste faengt hinter startpos an und geht bis
                  splitpos einschliesslich. Die zweite Liste faengt hinter
                  splitpos an und geht bis zu endpos einschliesslich.
                  Ist splitpos==endpos oder startpos==splitpos, so ist die
                  entsprechende Teilliste die eine leere Liste.
                  Zurueckgegeben wird ein Zeiger auf das (neue) letzte
                  Element der (neuen) Teilliste.
   Bemerkungen :  Diese Funktion wird fuer mergesort gebraucht. */
list_t* merge_sorted_sublists (list_t* head, list_t* startpos, 
            list_t* splitpos, list_t* endpos, int (*cmp_func)(void*, void*))
{
  list_t *i1=startpos->next, *i2=splitpos->next, *l,
         *enpos=endpos->next;

  if ((startpos==splitpos)||(splitpos==endpos)) return endpos;
  l=startpos;
  while (i1 || i2) {
    if ((!i2)||((i1&&i2)&&(cmp_func(i1->val,i2->val)<0))) {
      l->next=i1;
      i1->prev=l;
      l=l->next;
      if (i1!=splitpos) i1=i1->next;
      else i1=NULL;
    }
    else {
      l->next=i2;
      i2->prev=l;
      l=l->next;
      if (i2!=endpos) i2=i2->next;
      else i2=NULL;
    }
  }
  l->next=enpos;
  if (enpos) enpos->prev=l;
  else head->prev=l;
  return l;
}







/* Funktion:      void mergesort_list (list_t* l, 
                                  int (*cmp_func)(void*, void*));
   Beschreibung : sortiert die uebergebene Liste mit Hilfe des mergesort-
                  Algorithmus und der Vergleichsfunktion cmp_func. Es wird
                  lediglich neu verkettet. Der Algorithmus braucht also 
                  (im Gegensatz zu mergesort bei arrays) keinen neuen
                  Speicherplatz. */
void mergesort_list (list_t* l, int (*cmp_func)(void*, void*))
{
  list_t* mergesort_list_intern (list_t* startpos, list_t* endpos, long size)
    {
      long mid, i;
      list_t* midpos=startpos;

      if (startpos==endpos) return endpos;
      if (startpos->next==endpos) return endpos;

      mid=size/2;
      for (i=0; i<mid; i++) midpos=midpos->next;
      midpos = mergesort_list_intern (startpos,midpos,mid);
      endpos = mergesort_list_intern (midpos,  endpos,size-mid);
      return merge_sorted_sublists (l, startpos,midpos,endpos,cmp_func);
    }

  mergesort_list_intern (l,l->prev,list_size(l));
}




/* Funktion:      void swap_list_elements (list_t* head, 
                                     list_t* pos1, list_t* pos2);
   Beschreibung : Vertauscht die beiden Elemente pos1 und pos2 der Liste
                  mit dem Kopfelement head.
                  weder pos1 noch pos2 darf das Kopfelement der Liste sein. */
void swap_list_elements (list_t* head, list_t* pos1, list_t* pos2)
{
  /* bei der Implementierung nicht den Fall vergessen, wo pos1 und pos2
     benachbarte Elemente der Liste sind. Daher die vielen Variablen und
     Fallunterscheidungen. */
  list_t *p1next, *p2next, *p1prev, *p2prev, *tmp;


  if (pos1==pos2->next) {tmp=pos1; pos1=pos2; pos2=tmp;}

  p1next=pos1->next, p2next=pos2->next;
  p1prev=pos1->prev, p2prev=pos2->prev;

  pos1->prev->next = pos2;
  if (pos2->next) pos2->next->prev = pos1;
  else head->prev = pos1;

  if (pos2==pos1->next) {
    pos1->prev       = pos2;
    pos2->next       = pos1;
  }
  else {
    if (pos1->next) pos1->next->prev = pos2;
    else head->prev = pos2;
    pos2->prev->next = pos1;
    pos2->next       = p1next;
    pos1->prev       = p2prev;       
  }  
  pos1->next       = p2next;
  pos2->prev       = p1prev;

}


/* Funktion:      void bubble_sort_list (list_t* l, 
                               int (*cmp_func)(void*, void*));
   Beschreibung : Sortiert die Liste mit Hilfe eines Bubble-Sort-
                  aehnlichen Algorithmus und mit der Vergleichsfunktion 
                  cmp_func. Es wird lediglich neu verkettet. */
void bubblesort_list (list_t* l, int (*cmp_func)(void*, void*))
{
  list_t *i=l, *j, *tmp;
  while ((i=i->next))
    for (j=i->next; j!=NULL; j=j->next)
      if (cmp_func(j->val,i->val)<0) {
        swap_list_elements (l,i,j); 
         /* Die Listeneintraege sind vertauscht (d.h. die prev und next- 
            Zeiger sind neu gesetzt. i und j zeigen aber immer noch auf 
            die alten Stellen und muessen auch noch vertauscht werden. */
	tmp=i;
        i=j;
        j=tmp;
      }
}
