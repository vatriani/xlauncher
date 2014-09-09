#ifndef __LISTS_H__
#define __LISTS_H__

/*  Filename    : lists.h                                                *
 *  Author(s)   : Michael Becker,                                        *
 *  Date        : 12.04.2000                                             *
 *  Description : Listen-Operationen                                     */

/* This library is licensed under the GNU LGPL (Lesser General Public
   License) as defined at http://www.gnu.org.                            */


/* Listen sind hier doppelt verkettete Listen mit Kopfelement. Der Inhalt
   eines Listenelements ist ein void*-Pointer. Der Inhalt des Kopf-
   elementes ist NULL. */

typedef struct list_s {
  void *val;
  struct list_s *next, *prev;
} list_t;



list_t*  new_list (void);
list_t*  insert_list_element (list_t* head, list_t* pos, void *v);
list_t*  find_lastlesser_element (list_t *head, void *v,
                           int (*cmp_func)(void*, void*));
list_t*  insert_sorted_element (list_t* head, void *v,
                           int (*cmp_func)(void*, void*));
int      erase_list_element (list_t* head, list_t* pos,
                            void (*free_func)(void *));
list_t*  find_value (list_t *head, void *v, int (*cmp_func)(void*, void*));
list_t*  find_sorted_value (list_t *head, void *v,
                                    int (*cmp_func)(void*, void*));
void     clear_list (list_t* head, void (*free_func)(void *));
list_t*  copy_list (list_t *head, void* (*copy_func)(void*));
int      list_size (list_t *head);
void     concat_lists (list_t *l1, list_t* l2);
list_t*  splice_list (list_t* l, list_t* splitpos);
list_t*  merge_sorted_lists (list_t *l1, list_t* l2,
                            int (*cmp_func)(void*, void*));
list_t*  merge_sorted_sublists (list_t* head, list_t* startpos,
           list_t* splitpos, list_t* endpos, int (*cmp_func)(void*, void*));
void     mergesort_list (list_t* l, int (*cmp_func)(void*, void*));
void     swap_list_elements (list_t* head, list_t* pos1, list_t* pos2);
void     bubblesort_list (list_t* l, int (*cmp_func)(void*, void*));
int      list_t_cmp (list_t* head, list_t* l1, list_t* l2);
list_t*  next_n (list_t* head, list_t* pos, int n);
list_t*  prev_n (list_t* head, list_t* pos, int n);



#endif

